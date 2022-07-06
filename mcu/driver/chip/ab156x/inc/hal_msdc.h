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

#ifndef __HAL_MSDC_H__
#define __HAL_MSDC_H__

#include "hal_platform.h"
#include <string.h>
#include "stdio.h"
#include "hal_sd.h"
#include "hal_sd_internal.h"
#include "hal_sdio.h"
#include "hal_clock_platform.h"


#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#endif

#ifdef HAL_DVFS_MODULE_ENABLED
#include "hal_dvfs.h"
#include "hal_dvfs_internal.h"
#endif


//#define MSDC_DEBUG_FEATURE

#define SDIO_DEFAULT_MAX_SPEED    (25000)  /*The max speed is 25MHZ in default.*/
#define MSDC_MIN_SRC_CLK          (27000)  /*The mini src clk for 155x msdc is 26m,because of the analog signal,there will be some mean error*/

#define MSDC0_REG  ((MSDC_REGISTER_T *)SDIO_MASTER_BASE) /*defined in ab156x.h*/

#define MSDC_CMD_INTS       (MSDC_INT_CMDRDY_MASK | MSDC_INT_RSPCRCERR_MASK | MSDC_INT_CMDTMO_MASK)
#define MSDC_ACMD_INTS      (MSDC_INT_ACMDRDY_MASK | MSDC_INT_ACMDCRCERR_MASK  | MSDC_INT_ACMDTMO_MASK)
#define MSDC_DAT_INTS       (MSDC_INT_XFER_COMPL_MASK | MSDC_INT_DATCRCERR_MASK | MSDC_INT_DATTMO_MASK)

#define MSDC_CMD_INTS_EN    (MSDC_INTEN_CMDRDY_MASK | MSDC_INTEN_RSPCRCERR_MASK | MSDC_INTEN_CMDTMO_MASK)
#define MSDC_ACMD_INTS_EN   (MSDC_INTEN_ACMDRDY_MASK | MSDC_INTEN_ACMDCRCERR_MASK | MSDC_INTEN_ACMDTMO_MASK)
#define MSDC_DAT_INTS_EN    (MSDC_INTEN_XFER_COMPL_MASK | MSDC_INTEN_DATCRCERR_MASK | MSDC_INTEN_DATTMO_MASK)

#define MSDC_TXFIFOCNT(msdc_register_base)    ((msdc_register_base->MSDC_FIFOCS & MSDC_FIFOCS_TXCNT_MASK) >> 16)
#define MSDC_RXFIFOCNT(msdc_register_base)    ((msdc_register_base->MSDC_FIFOCS & MSDC_FIFOCS_RXCNT_MASK) >> 0)
#define MSDC_IS_SDC_BUSY(msdc_register_base)  (msdc_register_base->SDC_STS & SDC_STS_SDCBUSY_MASK)
#define MSDC_IS_CMD_BUSY(msdc_register_base)  (msdc_register_base->SDC_STS & SDC_STS_CMDBUSY_MASK)

/* This MACRO return 0 if condtion is meet, otherwise timeout happens */
#define MSDC_TIMEOUT_WAIT(cond, time) \
   do{\
        uint32_t start_count = 0;\
            uint32_t end_count = 0;\
        uint32_t count = 0;\
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &start_count);\
        do {\
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &end_count);\
            hal_gpt_get_duration_count(start_count, end_count, &count);\
            if ((cond))\
                break;\
        } while(count < time);\
    } while(0)

#define MSDC_STOP_DMA(msdc_register_base) \
    do { \
        msdc_register_base->DMA_CTRL |= MSDC_DMA_CTRL_STOP_MASK; \
        MSDC_TIMEOUT_WAIT(((!(msdc_register_base->DMA_CTRL & MSDC_DMA_CTRL_STOP_MASK)) || (!(msdc_register_base->DMA_CFG & MSDC_DMA_CFG_STS_MASK))), 500); \
    } while(0)


/*MSDC port, sd and sdio header file should defined as below*/
typedef enum {
    MSDC_PORT_0 = 0,
    MSDC_PORT_MAX
} msdc_port_t;

typedef enum {
    SDIO_STATE_DIS,     /*disabled, initalize,standby and inactive states -- card not selected*/
    SDIO_STATE_CMD,     /*data lines free, command waiting*/
    SDIO_STATE_TRN,     /*transfer*/
    SDIO_STATE_RFU      /*Reserved for Future Use*/
} sdio_state_t;

typedef struct {
    uint16_t vendor;
    uint16_t device;
} sdio_cis_t;

typedef enum {
    MSDC_BUS_WIDTH_1BITS      = 0,                                        /**< The SDIO bus width is 1 bit. */
    MSDC_SDIO_BUS_WIDTH_4BITS = 1,                                         /**< The SDIO bus width is 4 bits. */
} msdc_bus_width_t;

typedef enum {
    NO_DATA = 0,
    SINGLE_BLOCK = 1,
    MULTI_BLOCK = 2,
    STREAM_OPERATION = 3
} msdc_data_block_t;

typedef enum {
    MSDC_DMA_BURST_8_BYTES = 3,
    MSDC_DMA_BURST_16_BYTES = 4,
    MSDC_DMA_BURST_32_BYTES = 5,
    MSDC_DMA_BURST_64_BYTES = 6
} msdc_burst_type_t;

typedef enum {
    MSDC_OWNER_NONE = 0,
    MSDC_OWNER_SD   = 1,
    MSDC_OWNER_SDIO
} msdc_owner_t;


typedef struct {
    volatile msdc_owner_t    owner;
    uint32_t                 msdc_clock;
    uint32_t                 output_clock;
    uint32_t                 interrupt_register;
    volatile bool            is_card_present;
    volatile bool            is_initialized;
    volatile bool            is_timeout;
    volatile bool            is_card_plug_out;
    uint32_t                 command_status;
    uint32_t                 data_status;
    volatile uint32_t        dma_status;
    volatile uint32_t        dma_count;
    volatile uint32_t        dma_address;
    volatile bool            is_dma_write;
#if defined(HAL_SD_MODULE_ENABLED) && defined(HAL_SD_CARD_DETECTION)
    void (*msdc_card_detect_callback)(hal_sd_card_event_t event, void *user_data);
    void                     *card_detect_user_data;
#endif
#if defined(HAL_SD_MODULE_ENABLED)
    void (*msdc_sd_callback)(hal_sd_callback_event_t event, void *user_data);
#endif
#if defined(HAL_SDIO_MODULE_ENABLED)
    void (*msdc_sdio_dma_interrupt_callback)(hal_sdio_callback_event_t event, void *user_data);
    void (*msdc_sdio_interrupt_callback)(hal_sdio_callback_event_t event, void *user_data);
#endif
} msdc_config_t;

/*--------------------------------------------------------------------------*/
/* Descriptor Structure                                                     */
/*--------------------------------------------------------------------------*/
typedef struct {
    uint32_t  hwo: 1; /* could be changed by hw */
    uint32_t  bdp: 1;
    uint32_t  rsv0: 6;
    uint32_t  chksum: 8;
    uint32_t  intr: 1;
    uint32_t  rsv1: 15;
    void      *next;
    void      *ptr;
    uint32_t  buflen: 16;
    uint32_t  extlen: 8;
    uint32_t  rsv2: 8;
    uint32_t  arg;
    uint32_t  blknum;
    uint32_t  cmd;
} gpd_t;

typedef struct {
    uint32_t  eol: 1;
    uint32_t  rsv0: 7;
    uint32_t  chksum: 8;
    uint32_t  rsv1: 1;
    uint32_t  blkpad: 1;
    uint32_t  dwpad: 1;
    uint32_t  rsv2: 13;
    void      *next;
    void      *ptr;
    uint32_t  buflen: 16;
    uint32_t  rsv3: 16;
} bd_t;

typedef enum {
    MSDC_FAIL = -1,
    MSDC_OK = 0,
    MSDC_INITIALIZED = 1
} msdc_status_t;

#define MSDC_FIFO_SIZE (128)
#define MSDC_MAX_BLOCK_LENGTH (2048)
#define MSDC_DATA_TIMEOUT_COUNT               (80)
#define MSDC_INIT_CLOCK      240/*KHz*/
#define MSDC_OUTPUT_CLOCK    15300

#ifdef HAL_DVFS_MODULE_ENABLED
void msdc_dvfs_lock_current(msdc_port_t msdc_port);
#endif
void msdc_wait(uint32_t wait_ms);
void msdc_power_set(msdc_port_t msdc_port, bool is_power_on);
void msdc_reset(msdc_port_t msdc_port);
void msdc_clear_fifo(msdc_port_t msdc_port);
void msdc_set_bus_width(msdc_port_t msdc_port, msdc_bus_width_t bus_width);
void msdc_set_output_clock(msdc_port_t msdc_port, uint32_t clock);
uint32_t msdc_get_output_clock(msdc_port_t msdc_port);
void msdc_sdio_interrupt_set(msdc_port_t msdc_port, bool enable);
void msdc_data_interrupt_handle(msdc_port_t msdc_port, uint32_t status);
void msdc_command_interrupt_handle(msdc_port_t msdc_port, uint32_t status);
void msdc_sdio_interrupt_handle(msdc_port_t msdc_port);
void msdc_nvic_set(msdc_port_t msdc_port, bool enable);
void msdc_interrupt_init(msdc_port_t msdc_port);
void msdc_eint_isr(void *user_data);
void msdc_eint_registration(msdc_port_t msdc_port);
msdc_status_t msdc_init(msdc_port_t msdc_port, msdc_bus_width_t bus_width);
void msdc_deinit(msdc_port_t msdc_port);
bool msdc_card_is_present(msdc_port_t msdc_port);
void msdc_dma_enable(msdc_port_t msdc_port);
void msdc_dma_disable(msdc_port_t msdc_port);
void msdc_dma_stop(msdc_port_t msdc_port);
msdc_owner_t msdc_get_owner(msdc_port_t msdc_port);
void msdc_set_owner(msdc_port_t msdc_port, msdc_owner_t owner);
void msdc_card_power_set(msdc_port_t msdc_port, bool is_power_on);
void msdc_clock_init(msdc_port_t msdc_port);
void msdc_set_burst_type(msdc_port_t msdc_port, msdc_burst_type_t burst_type);
void msdc_dump_all_reg(msdc_port_t msdc_port);
void msdc_io_config(msdc_port_t msdc_port, msdc_bus_width_t bus_width);

#endif /*__HAL_MSDC_H__*/

