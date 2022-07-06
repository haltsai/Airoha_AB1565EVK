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
#include "hal_gpio.h"
#if defined(HAL_SD_MODULE_ENABLED) || defined(HAL_SDIO_MODULE_ENABLED)
#include "hal_log.h"
#include "hal_clock_internal.h"
#include "hal_gpio_internal.h"
#include "hal_eint.h"
#include "assert.h"
#include "msdc_custom_config.h"
#include "hal_sd_internal.h"
#include "hal_sdio_internal.h"
#include "hal_platform.h"

msdc_config_t msdc_config[MSDC_PORT_MAX];

/*ept tool config*/
extern const char HAL_MSDC_0_CK_PIN;
extern const char HAL_MSDC_0_CM_PIN;
extern const char HAL_MSDC_0_DA0_PIN;
extern const char HAL_MSDC_0_DA1_PIN;
extern const char HAL_MSDC_0_DA2_PIN;
extern const char HAL_MSDC_0_DA3_PIN;

#if defined(HAL_SD_MODULE_ENABLED) && defined(HAL_SD_CARD_DETECTION)
static volatile bool msdc_card_detection_eint_polarity;
extern const unsigned char HAL_MSDC_EINT;  /*ept tool config*/
#endif

#ifdef HAL_SLEEP_MANAGER_ENABLED
extern sleep_management_lock_request_t sd_sleep_handle[];
#endif

extern uint32_t hal_clock_get_freq_meter(hal_src_clock clock_src_id, uint32_t winset);

uint32_t input_freq;

#ifdef HAL_DVFS_MODULE_ENABLED
/*  In order to ensure that there is no error in the data transmission process,  need lock current frequency  */
/*  If the vcore voltage changes, the MSDC source clock should change accordingly.  */
/*  MSDC_CLK      vcore_voltage      CPU_CLK  */
/*    52MHz            >=0.8V         >=104MHz  */
/*    26MHz       0.73v or 0.75V        52MHz  */
void msdc_dvfs_lock_current(msdc_port_t msdc_port)
{
    pmu_power_vcore_voltage_t vol;

    /* lock current frequency  */
    hal_dvfs_lock_level(HAL_DVFS_LOCK);

    /* get current vcore voltage  */
    vol = pmu_get_vcore_voltage();
    if (vol >= PMIC_VCORE_0P8_V) {
        clock_mux_sel(CLK_SDIOMST0_SEL, 2);   //52M
        msdc_config[msdc_port].msdc_clock = 52000;
        msdc_set_output_clock(msdc_port, input_freq);
    } else if ((vol == PMIC_VCORE_0P73_V) || (vol == PMIC_VCORE_0P75_V)) {
        clock_mux_sel(CLK_SDIOMST0_SEL, 0);    //26M
        msdc_config[msdc_port].msdc_clock = 26000;
        msdc_set_output_clock(msdc_port, input_freq);
    } else {
        log_hal_msgid_error("Vcore voltage not enough，MSDC is dangerous to use\r\n", 0);
    }
}
#endif

void msdc_wait(uint32_t wait_ms)
{
    hal_gpt_delay_ms(wait_ms);
}

void msdc_power_set(msdc_port_t msdc_port, bool is_power_on)
{
    hal_clock_cg_id clk_id;
    hal_clock_cg_id clk_id_ex;

    clk_id = (MSDC_PORT_0 == msdc_port) ? HAL_CLOCK_CG_SDIOMST0 : HAL_CLOCK_CG_SDIOMST0;
    clk_id_ex = (MSDC_PORT_0 == msdc_port) ? HAL_CLOCK_CG_SDIOMST : HAL_CLOCK_CG_SDIOMST;

    if (is_power_on) {
        hal_clock_enable(clk_id);
        hal_clock_enable(clk_id_ex);
    } else {
        hal_clock_disable(clk_id);
        hal_clock_disable(clk_id_ex);
    }
}

void msdc_reset(msdc_port_t msdc_port)
{
    MSDC_REGISTER_T *msdc_register_base;

    msdc_register_base = MSDC0_REG;

    if (!(msdc_register_base->MSDC_CFG & MSDC_CFG_RST_MASK)) {
        msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG | MSDC_CFG_RST_MASK;
    }

    while (msdc_register_base->MSDC_CFG & MSDC_CFG_RST_MASK);
}

void msdc_clear_fifo(msdc_port_t msdc_port)
{
    MSDC_REGISTER_T *msdc_register_base;

    msdc_register_base = MSDC0_REG;

    msdc_register_base->MSDC_FIFOCS = msdc_register_base->MSDC_FIFOCS | MSDC_FIFOCS_CLR_MASK;

    while (msdc_register_base->MSDC_FIFOCS & MSDC_FIFOCS_CLR_MASK);
}

void msdc_set_bus_width(msdc_port_t msdc_port, msdc_bus_width_t bus_width)
{
    MSDC_REGISTER_T *msdc_register_base;

    msdc_register_base = MSDC0_REG;

    msdc_register_base->SDC_CFG = (msdc_register_base->SDC_CFG & (~SDC_CFG_BUSWIDTH_MASK)) | (bus_width << SDC_CFG_BUSWIDTH_OFFSET);
}

extern sd_information_t sd_information;
void msdc_set_output_clock(msdc_port_t msdc_port, uint32_t clock)
{
    uint32_t clock_config = 0;
    MSDC_REGISTER_T *msdc_register_base;
    hal_clock_cg_id clk_id;
    uint32_t start_count = 0;
    uint32_t end_count = 0;
    uint32_t count = 0;
    uint32_t reg_val = 0;
    uint32_t msdc_src_clk_freq = 0;

    msdc_register_base = MSDC0_REG;
    clk_id = (MSDC_PORT_0 == msdc_port) ? HAL_CLOCK_CG_SDIOMST : HAL_CLOCK_CG_SDIOMST;

    /*get src clk freq config*/
    msdc_src_clk_freq = msdc_config[msdc_port].msdc_clock / 1000 * 1000;

    /*disable msdc clock*/
    hal_clock_disable(clk_id);

#ifdef MSDC_DEBUG_FEATURE
    log_hal_msgid_info("src clk = %d\r\n", 1, msdc_src_clk_freq);
#endif

    /*config sdio clk depend on src clk freq*/
    if (clock >= msdc_src_clk_freq) {
        msdc_register_base->MSDC_CFG |= MSDC_CFG_CCKMD_MASK;
        msdc_config[msdc_port].output_clock = msdc_config[msdc_port].msdc_clock;
    } else if (clock >= (msdc_src_clk_freq >> 1)) {
        msdc_register_base->MSDC_CFG &= ~MSDC_CFG_CCKMD_MASK;
        msdc_register_base->MSDC_CFG &= ~MSDC_CFG_CKDIV_MASK;
        msdc_config[msdc_port].output_clock = msdc_config[msdc_port].msdc_clock >> 1;
    } else {
        clock_config = ((msdc_src_clk_freq + clock - 1) / clock);
        clock_config = (clock_config >> 2) + (((clock_config & 3) != 0) ? 1 : 0);
        msdc_config[msdc_port].output_clock = msdc_config[msdc_port].msdc_clock / (4 * clock_config);

        msdc_register_base->MSDC_CFG = (msdc_register_base->MSDC_CFG & (~MSDC_CFG_CKDIV_MASK)) | ((clock_config & 0xff) << 8);
    }

    /*config cmd response data sample edge.*/
    if (msdc_config[msdc_port].output_clock > MSDC_MIN_SRC_CLK) {
        msdc_register_base->MSDC_IOCON &= ~MSDC_IOCON_RSPL_MASK;
        msdc_register_base->MSDC_IOCON &= ~MSDC_IOCON_DSPL_MASK;
        msdc_register_base->MSDC_IOCON &= ~MSDC_IOCON_WDSPL_MASK;
    } else {
        if (sd_information.card_type == HAL_SD_TYPE_MMC_CARD ||
            sd_information.card_type == HAL_SD_TYPE_MMC42_CARD) {
            msdc_register_base->MSDC_IOCON &= ~MSDC_IOCON_RSPL_MASK;
            msdc_register_base->MSDC_IOCON &= ~MSDC_IOCON_DSPL_MASK;
            msdc_register_base->MSDC_IOCON &= ~MSDC_IOCON_WDSPL_MASK;
        } else {
            msdc_register_base->MSDC_IOCON |= MSDC_IOCON_RSPL_MASK;
            msdc_register_base->MSDC_IOCON |= MSDC_IOCON_DSPL_MASK;
            msdc_register_base->MSDC_IOCON |= MSDC_IOCON_WDSPL_MASK;
        }
    }

#ifdef MSDC_DEBUG_FEATURE
    log_hal_msgid_info("setting clock is %d, MSDC_CFG = %x \r\n", 2, clock, msdc_register_base->MSDC_CFG);
#endif

    /*enable msdc clock*/
    hal_clock_enable(clk_id);

    /*wait msdc clk stable*/
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &start_count);
    do {
        reg_val = msdc_register_base->MSDC_CFG;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &end_count);
        hal_gpt_get_duration_count(start_count, end_count, &count);
    } while ((!(reg_val & MSDC_CFG_CKSTB_MASK)) || (count >= MSDC_TIMEOUT_PERIOD_INIT));

    if (count >= MSDC_TIMEOUT_PERIOD_INIT) {
        log_hal_msgid_error("msdc clk setting error!msdc clk is not stable\r\n", 0);
    }
}


uint32_t msdc_get_output_clock(msdc_port_t msdc_port)
{
    return (msdc_config[msdc_port].output_clock);
}


void msdc_sdio_interrupt_set(msdc_port_t msdc_port, bool enable)
{
    MSDC_REGISTER_T *msdc_register_base;

    msdc_register_base = MSDC0_REG;

    if (enable) {
        msdc_register_base->MSDC_INTEN |= MSDC_INT_SDIOIRQ_MASK;
    } else {
        msdc_register_base->MSDC_INTEN &= ~MSDC_INT_SDIOIRQ_MASK;
    }
}

void msdc_data_interrupt_handle(msdc_port_t msdc_port, uint32_t status)
{
#if defined(HAL_SD_MODULE_ENABLED)
    if (MSDC_OWNER_SD == msdc_get_owner(msdc_port)) {
        if (status & MSDC_INT_XFER_COMPL_MASK) {
            sd_wait_dma_interrupt_transfer_ready((hal_sd_port_t)msdc_port);
        } else if ((status & MSDC_INT_DATTMO_MASK) || (status & MSDC_INT_DATCRCERR_MASK)) {
            if (msdc_config[msdc_port].msdc_sd_callback) {
                /*call user callback to inform transfer error*/
                if (status & MSDC_INT_DATTMO_MASK) {
                    msdc_config[msdc_port].msdc_sd_callback(HAL_SD_EVENT_DATA_TIMEOUT, (void *)0);
                } else if (status & MSDC_INT_DATCRCERR_MASK) {
                    msdc_config[msdc_port].msdc_sd_callback(HAL_SD_EVENT_CRC_ERROR, (void *)0);
                }
            }
        }
    }
#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_unlock_sleep(sd_sleep_handle[msdc_port]);
#endif
#endif

#if defined(HAL_SDIO_MODULE_ENABLED)
    if (MSDC_OWNER_SDIO == msdc_get_owner(msdc_port)) {
        if (status & MSDC_INT_XFER_COMPL_MASK) {
            sdio_wait_dma_interrupt_transfer_ready((hal_sdio_port_t)msdc_port);
        } else if ((status & MSDC_INT_DATTMO_MASK) || (status & MSDC_INT_DATCRCERR_MASK)) {
            if (msdc_config[msdc_port].msdc_sdio_dma_interrupt_callback) {
                /*call user callback to inform transfer error*/
                if (status & MSDC_INT_DATTMO_MASK) {
                    msdc_config[msdc_port].msdc_sdio_dma_interrupt_callback(HAL_SDIO_EVENT_DATA_TIMEOUT, (void *)0);
                } else if (status & MSDC_INT_DATCRCERR_MASK) {
                    msdc_config[msdc_port].msdc_sdio_dma_interrupt_callback(HAL_SDIO_EVENT_CRC_ERROR, (void *)0);
                }
            }
        }
    }
#endif
}

void msdc_command_interrupt_handle(msdc_port_t msdc_port, uint32_t status)
{

}

void msdc_sdio_interrupt_handle(msdc_port_t msdc_port)
{
#if defined(HAL_SDIO_MODULE_ENABLED)
    if (MSDC_OWNER_SDIO == msdc_get_owner(msdc_port)) {
        if (msdc_config[msdc_port].msdc_sdio_interrupt_callback) {
            msdc_config[msdc_port].msdc_sdio_interrupt_callback(HAL_SDIO_EVENT_SUCCESS, (void *)0);
        } else {
            log_hal_msgid_error("sdio interrupt callback is null!\r\n", 0);
        }
    }
#endif
}

void msdc_isr(hal_nvic_irq_t irq_number)
{
    uint32_t command_status  = MSDC_INT_CMDRDY_MASK | MSDC_INT_CMDTMO_MASK | MSDC_INT_RSPCRCERR_MASK;
    uint32_t acommand_status = MSDC_INT_ACMDRDY_MASK | MSDC_INT_ACMDTMO_MASK | MSDC_INT_ACMDCRCERR_MASK | MSDC_INT_ACMD19_DONE_MASK;
    uint32_t data_status  = MSDC_INT_XFER_COMPL_MASK | MSDC_INT_DXFER_DONE_MASK | MSDC_INT_DATCRCERR_MASK | MSDC_INT_DATTMO_MASK;
    uint32_t msdc_int;
    uint32_t msdc_inten;
    uint32_t interrupt_status;

    msdc_nvic_set(MSDC_PORT_0, false);

    msdc_int = MSDC0_REG->MSDC_INT;
    msdc_inten = MSDC0_REG->MSDC_INTEN;
    interrupt_status = msdc_int & msdc_inten;

    MSDC0_REG->MSDC_INT |= interrupt_status;

    //log_hal_info("irq handler got msdc_int=%08X msdc_inten=%08X => %08X (in interrupt)\r\n", (unsigned int)msdc_int, (unsigned int)msdc_inten, (unsigned int)interrupt_status);

    /* transfer complete interrupt */
    if (interrupt_status & data_status) {
        msdc_data_interrupt_handle(MSDC_PORT_0, interrupt_status);
    }

    /* command interrupts */
    if (interrupt_status & (command_status | acommand_status)) {
        msdc_command_interrupt_handle(MSDC_PORT_0, interrupt_status);
    }

    /* sdio interrupt */
    if (interrupt_status & MSDC_INT_SDIOIRQ_MASK) {
        msdc_sdio_interrupt_handle(MSDC_PORT_0);
    }

    msdc_nvic_set(MSDC_PORT_0, true);
}

void msdc_nvic_set(msdc_port_t msdc_port, bool enable)
{
    hal_nvic_irq_t irq_number;

    irq_number = SDIO_MST0_IRQn;

    /*should call nvic api to set msdc interrupt enable or disable*/
    if (enable) {
        hal_nvic_enable_irq(irq_number);
    } else {
        hal_nvic_disable_irq(irq_number);
    }
}

void msdc_interrupt_init(msdc_port_t msdc_port)
{
    uint32_t reg_value = 0;
    MSDC_REGISTER_T *msdc_register_base;
    hal_nvic_irq_t irq_number;

    msdc_register_base = MSDC0_REG;
    irq_number = SDIO_MST0_IRQn;

    msdc_nvic_set(msdc_port, false);

    if (MSDC_PORT_0 == msdc_port) {
        hal_nvic_register_isr_handler(irq_number, msdc_isr);
    } else {
        log_hal_msgid_error("msdc_interrupt_init:port error!\r\n", 0);
    }

    /* Disable and clear all interrupts */
    reg_value = msdc_register_base->MSDC_INTEN;
    msdc_register_base->MSDC_INTEN &= ~reg_value;
    reg_value = msdc_register_base->MSDC_INT;
    msdc_register_base->MSDC_INT |= reg_value;

    msdc_nvic_set(msdc_port, true);
}

#if defined(HAL_SD_MODULE_ENABLED) && defined(HAL_SD_CARD_DETECTION)
void msdc_eint_isr(void *user_data)
{
    hal_eint_trigger_mode_t mode;
    hal_sd_card_event_t sd_event = HAL_SD_EVENT_CARD_INSERTED;

    hal_eint_mask((hal_eint_number_t)HAL_MSDC_EINT);

    msdc_config[MSDC_PORT_0].is_card_present = msdc_card_detection_eint_polarity ? false : true;

    msdc_card_detection_eint_polarity = msdc_card_detection_eint_polarity ? false : true;

    mode = msdc_card_detection_eint_polarity ? HAL_EINT_LEVEL_HIGH : HAL_EINT_LEVEL_LOW;
    hal_eint_set_trigger_mode((hal_eint_number_t)HAL_MSDC_EINT, mode);

    hal_eint_unmask((hal_eint_number_t)HAL_MSDC_EINT);

    if (msdc_config[MSDC_PORT_0].is_card_present == false) {
        msdc_config[MSDC_PORT_0].is_card_plug_out = true;
        sd_event = HAL_SD_EVENT_CARD_REMOVED;
        msdc_reset(MSDC_PORT_0);
        msdc_dma_disable(MSDC_PORT_0);
        msdc_deinit(MSDC_PORT_0);
    } else {
        msdc_config[MSDC_PORT_0].is_card_plug_out = false;
        sd_event = HAL_SD_EVENT_CARD_INSERTED;
    }

    if (msdc_config[MSDC_PORT_0].msdc_card_detect_callback != NULL) {
        msdc_config[MSDC_PORT_0].msdc_card_detect_callback(sd_event, msdc_config[MSDC_PORT_0].card_detect_user_data);
    }
}

void msdc_eint_registration(msdc_port_t msdc_port)
{
    hal_eint_config_t config;
    unsigned char hal_msdc_eint;

    hal_msdc_eint = (MSDC_PORT_0 == msdc_port) ? HAL_MSDC_EINT : HAL_MSDC_EINT;

    /*HAL_MSDC_EINT is EPT tool config, the HAL_MSDC_EINT value is 0xff means the hot plug eint is not configured in EPT tool*/
    if (0xFF == hal_msdc_eint) {
        assert(0);
    }

    msdc_card_detection_eint_polarity = false;

    config.debounce_time = 500;
    config.trigger_mode = HAL_EINT_LEVEL_LOW;

    hal_eint_mask((hal_eint_number_t)hal_msdc_eint);

    if (HAL_EINT_STATUS_OK != hal_eint_init((hal_eint_number_t)hal_msdc_eint, &config)) {
        log_hal_msgid_error("ERROR:hal_eint_init error!\r\n", 0);
    }

    if (MSDC_PORT_0 == msdc_port) {
        if (HAL_EINT_STATUS_OK != hal_eint_register_callback((hal_eint_number_t)hal_msdc_eint, msdc_eint_isr, NULL)) {
            log_hal_msgid_error("ERROR:hal_eint_register_callback error!\r\n", 0);
        }
    }
    hal_eint_unmask((hal_eint_number_t)hal_msdc_eint);
}
#endif

void msdc_io_config(msdc_port_t msdc_port, msdc_bus_width_t bus_width)
{
    if (MSDC_BUS_WIDTH_1BITS == bus_width) {
        /*IO driving setting.*/
        hal_gpio_set_driving_current(HAL_MSDC_0_CK_PIN, (hal_gpio_driving_current_t)MSDC_DATA_LINE_DRIVING_CAPABILITY);
        hal_gpio_set_driving_current(HAL_MSDC_0_CM_PIN, (hal_gpio_driving_current_t)MSDC_DATA_LINE_DRIVING_CAPABILITY);
        hal_gpio_set_driving_current(HAL_MSDC_0_DA0_PIN, (hal_gpio_driving_current_t)MSDC_DATA_LINE_DRIVING_CAPABILITY);
    } else if (MSDC_SDIO_BUS_WIDTH_4BITS == bus_width) {
        /*IO driving setting.*/
        hal_gpio_set_driving_current(HAL_MSDC_0_CK_PIN, (hal_gpio_driving_current_t)MSDC_DATA_LINE_DRIVING_CAPABILITY);
        hal_gpio_set_driving_current(HAL_MSDC_0_CM_PIN, (hal_gpio_driving_current_t)MSDC_DATA_LINE_DRIVING_CAPABILITY);
        hal_gpio_set_driving_current(HAL_MSDC_0_DA0_PIN, (hal_gpio_driving_current_t)MSDC_DATA_LINE_DRIVING_CAPABILITY);
        hal_gpio_set_driving_current(HAL_MSDC_0_DA1_PIN, (hal_gpio_driving_current_t)MSDC_DATA_LINE_DRIVING_CAPABILITY);
        hal_gpio_set_driving_current(HAL_MSDC_0_DA2_PIN, (hal_gpio_driving_current_t)MSDC_DATA_LINE_DRIVING_CAPABILITY);
        hal_gpio_set_driving_current(HAL_MSDC_0_DA3_PIN, (hal_gpio_driving_current_t)MSDC_DATA_LINE_DRIVING_CAPABILITY);
    }
}

msdc_status_t msdc_init(msdc_port_t msdc_port, msdc_bus_width_t bus_width)
{
    msdc_status_t status = MSDC_OK;
    uint32_t reg_value = 0;
    MSDC_REGISTER_T *msdc_register_base;
    uint32_t msdc_src_clk_freq = 0;

    /*if MSDC have initialized, return directly*/
    if (msdc_config[msdc_port].is_initialized) {
        return MSDC_INITIALIZED;
    }

    msdc_register_base = MSDC0_REG;

    msdc_power_set(msdc_port, true);

    msdc_io_config(msdc_port, bus_width);

    /* make msdc clk source 52m */
    clock_mux_sel(CLK_SDIOMST0_SEL, 2);     /*  52m  */
    /*  clock_mux_sel(CLK_SDIOMST0_SEL, 0);   26m */

    /*get src clk freq config*/
    msdc_src_clk_freq = hal_clock_get_freq_meter(_hf_fsdiomst0_ck, 100);

    msdc_config[msdc_port].msdc_clock = msdc_src_clk_freq;

    /* Reset */
    msdc_reset(msdc_port);
    msdc_clear_fifo(msdc_port);

    /* Disable card detection */
    msdc_register_base->MSDC_PS = msdc_register_base->MSDC_PS & (~MSDC_PS_CDEN_MASK);

    /* Disable and clear all interrupts */
    reg_value = msdc_register_base->MSDC_INTEN;
    msdc_register_base->MSDC_INTEN &= ~reg_value;
    reg_value = msdc_register_base->MSDC_INT;
    msdc_register_base->MSDC_INT |= reg_value;

    msdc_interrupt_init(msdc_port);

    /* Configure to PIO mode */
    msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG | MSDC_CFG_PIO_MASK;

    /* Configure to MMC/SD mode */
    msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG | MSDC_CFG_MODE_MASK;

    /* write crc timeout detection */
    msdc_register_base->PATCH_BIT0 = msdc_register_base->PATCH_BIT0 | (1 << 30);

    /*switch INCR1 to single burst.*/
    msdc_register_base->PATCH_BIT1 |= (1 << 16);

    /* Configure to default data timeout */
    msdc_register_base->SDC_CFG = (msdc_register_base->SDC_CFG & (~SDC_CFG_DTOC_MASK)) | (80 << 24);

    msdc_set_bus_width(msdc_port, MSDC_BUS_WIDTH_1BITS);

    /*set block number.*/
    msdc_register_base->SDC_BLK_NUM = 1;

    msdc_config[msdc_port].is_card_present = true;

#if defined(HAL_SD_MODULE_ENABLED) && defined(HAL_SD_CARD_DETECTION)
    /*card detection eint registration*/
    msdc_eint_registration(msdc_port);
    log_hal_msgid_info("msdc eint register done\r\n", 0);
#endif

    log_hal_msgid_info("init hardware done\r\n", 0);

    return status;
}

void msdc_deinit(msdc_port_t msdc_port)
{
    uint32_t reg_value = 0;
    MSDC_REGISTER_T *msdc_register_base;

    msdc_register_base = MSDC0_REG;

    /* Disable and clear all interrupts */
    reg_value = msdc_register_base->MSDC_INTEN;
    msdc_register_base->MSDC_INTEN &= ~reg_value;
    reg_value = msdc_register_base->MSDC_INT;
    msdc_register_base->MSDC_INT |= reg_value;
    msdc_power_set(msdc_port, false);

    msdc_config[msdc_port].is_card_present = false;

    log_hal_msgid_info("deinit hardware done", 0);
}


bool msdc_card_is_present(msdc_port_t msdc_port)
{
    return msdc_config[msdc_port].is_card_present;
}

void msdc_dma_enable(msdc_port_t msdc_port)
{
    MSDC_REGISTER_T *msdc_register_base;

    msdc_register_base = MSDC0_REG;

    msdc_register_base->MSDC_CFG &= ~MSDC_CFG_PIO_MASK;
}

void msdc_dma_disable(msdc_port_t msdc_port)
{
    MSDC_REGISTER_T *msdc_register_base;

    msdc_register_base = MSDC0_REG;

    msdc_register_base->MSDC_CFG |= MSDC_CFG_PIO_MASK;
}

void msdc_dma_stop(msdc_port_t msdc_port)
{
    uint32_t wints = MSDC_INTEN_XFER_COMPL_MASK |    /* To detect one GPD data transfer complete */
                     MSDC_INTEN_DATTMO_MASK |        /* To detect data transfer timdout */
                     MSDC_INTEN_DATCRCERR_MASK;      /* To detect data transfer error */
    MSDC_REGISTER_T *msdc_register_base;

    msdc_register_base = MSDC0_REG;

    log_hal_msgid_info("***dma status: 0x%08x", 1, (unsigned int)msdc_register_base->DMA_CFG);
    MSDC_STOP_DMA(msdc_register_base);

    msdc_register_base->MSDC_INTEN &= ~wints;
    msdc_register_base->MSDC_INT |= wints;

    log_hal_msgid_info("***dma stop...", 0);
}

msdc_owner_t msdc_get_owner(msdc_port_t msdc_port)
{
    return (msdc_config[msdc_port].owner);
}

void msdc_set_owner(msdc_port_t msdc_port, msdc_owner_t owner)
{
    msdc_config[msdc_port].owner = owner;
}

/*this function used to turn on power for card VDD and VDDQ*/
void msdc_card_power_set(msdc_port_t msdc_port, bool is_power_on)
{
    /*card power set.*/
}

void msdc_clock_init(msdc_port_t msdc_port)
{
    MSDC_REGISTER_T *msdc_register_base;

    msdc_register_base = MSDC0_REG;

    /*in 50MHZ case, we should set 80 to have at least 100ms timeout,for initial read*/
    msdc_register_base->SDC_CFG = (msdc_register_base->SDC_CFG & (~SDC_CFG_DTOC_MASK)) |
                                  (MSDC_DATA_TIMEOUT_COUNT << SDC_CFG_DTOC_OFFSET);

    /*set output clock to 240KHz. The clock should <= 400KHz,*/
    msdc_set_output_clock(msdc_port, MSDC_INIT_CLOCK);

}

void msdc_set_burst_type(msdc_port_t msdc_port, msdc_burst_type_t burst_type)
{
    MSDC_REGISTER_T *msdc_register_base;

    msdc_register_base = MSDC0_REG;

    msdc_register_base->DMA_CTRL = (msdc_register_base->DMA_CTRL & (~MSDC_DMA_CTRL_BRUSTSZ_MASK)) | (burst_type << 12);
}

#ifdef MSDC_DEBUG_FEATURE
void msdc_dump_all_reg(msdc_port_t msdc_port)
{
    MSDC_REGISTER_T *msdc_register_base;

    msdc_register_base = MSDC0_REG;

    log_hal_msgid_info("MSDC_CFG:0x%08x\r\n", 1, msdc_register_base->MSDC_CFG);
    log_hal_msgid_info("MSDC_IOCON:0x%08x\r\n", 1, msdc_register_base->MSDC_IOCON);
    log_hal_msgid_info("MSDC_PS:0x%08x\r\n", 1, msdc_register_base->MSDC_PS);
    log_hal_msgid_info("MSDC_INT:0x%08x\r\n", 1, msdc_register_base->MSDC_INT);
    log_hal_msgid_info("MSDC_INTEN:0x%08x\r\n", 1, msdc_register_base->MSDC_INTEN);
    log_hal_msgid_info("MSDC_FIFOCS:0x%08x\r\n", 1, msdc_register_base->MSDC_FIFOCS);
    log_hal_msgid_info("MSDC_TXDATA:0x%08x\r\n", 1, msdc_register_base->MSDC_TXDATA);
    log_hal_msgid_info("MSDC_RXDATA:0x%08x\r\n", 1, msdc_register_base->MSDC_RXDATA);
    log_hal_msgid_info("SDC_CFG:0x%08x\r\n", 1, msdc_register_base->SDC_CFG);
    log_hal_msgid_info("SDC_CMD:0x%08x\r\n", 1, msdc_register_base->SDC_CMD);
    log_hal_msgid_info("SDC_ARG:0x%08x\r\n", 1, msdc_register_base->SDC_ARG);
    log_hal_msgid_info("SDC_STS:0x%08x\r\n", 1, msdc_register_base->SDC_STS);
    log_hal_msgid_info("SDC_RESP0:0x%08x\r\n", 1, msdc_register_base->SDC_RESP0);
    log_hal_msgid_info("SDC_RESP1:0x%08x\r\n", 1, msdc_register_base->SDC_RESP1);
    log_hal_msgid_info("SDC_RESP2:0x%08x\r\n", 1, msdc_register_base->SDC_RESP2);
    log_hal_msgid_info("SDC_RESP3:0x%08x\r\n", 1, msdc_register_base->SDC_RESP3);
    log_hal_msgid_info("SDC_BLK_NUM:0x%08x\r\n", 1, msdc_register_base->SDC_BLK_NUM);
    log_hal_msgid_info("SDC_CSTS:0x%08x\r\n", 1, msdc_register_base->SDC_CSTS);
    log_hal_msgid_info("SDC_CSTS_EN:0x%08x\r\n", 1, msdc_register_base->SDC_CSTS_EN);
    log_hal_msgid_info("SDC_DATCRC_STS:0x%08x\r\n", 1, msdc_register_base->SDC_DATCRC_STS);
    log_hal_msgid_info("SDC_ACMD_RESP:0x%08x\r\n", 1, msdc_register_base->SDC_ACMD_RESP);
    log_hal_msgid_info("DMA_SA:0x%08x\r\n", 1, msdc_register_base->DMA_SA);
    log_hal_msgid_info("DMA_CA:0x%08x\r\n", 1, msdc_register_base->DMA_CA);
    log_hal_msgid_info("DMA_CTRL:0x%08x\r\n", 1, msdc_register_base->DMA_CTRL);
    log_hal_msgid_info("DMA_CFG:0x%08x\r\n", 1, msdc_register_base->DMA_CFG);
    log_hal_msgid_info("SW_DBG_SEL:0x%08x\r\n", 1, msdc_register_base->SW_DBG_DEL);
    log_hal_msgid_info("SW_DBG_OUT:0x%08x\r\n", 1, msdc_register_base->SW_DBG_OUT);
    log_hal_msgid_info("DMA_LENGTH:0x%08x\r\n", 1, msdc_register_base->DMA_LENGTH);
    log_hal_msgid_info("PATCH_BIT0:0x%08x\r\n", 1, msdc_register_base->PATCH_BIT0);
    log_hal_msgid_info("PATCH_BIT1:0x%08x\r\n", 1, msdc_register_base->PATCH_BIT1);
    log_hal_msgid_info("PAD_TUNE:0x%08x\r\n", 1, msdc_register_base->PAD_TUNE);
    log_hal_msgid_info("DAT_RD_DLY0:0x%08x\r\n", 1, msdc_register_base->DAT_RD_DLY0);
    log_hal_msgid_info("DAT_RD_DLY1:0x%08x\r\n", 1, msdc_register_base->DAT_RD_DLY1);
    log_hal_msgid_info("MAIN_VER:0x%08x\r\n", 1, msdc_register_base->MAIN_VER);
    log_hal_msgid_info("ECO_VER:0x%08x\r\n", 1, msdc_register_base->ECO_VER);
}
#endif

uint8_t msdc_cal_checksum(const void *buf, uint32_t len)
{
    uint32_t i = 0;
    uint32_t sum = 0;

    char *data = (char *)buf;

    for (i = 0; i < len; i++) {
        sum += *data++;
    }
    return 0xFF - (uint8_t)sum;
}


#endif /*HAL_SD_MODULE_ENABLED || HAL_SDIO_MODULE_ENABLED*/

