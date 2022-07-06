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

#ifndef __HAL_ESC_INTERNAL_H__
#define __HAL_ESC_INTERNAL_H__

#include "hal_platform.h"

#ifdef HAL_ESC_MODULE_ENABLED

/* sleep manager related header files  */
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"

#include "esc_device_config.h"

#if defined(ESC_FLASH_ENABLE) && defined(ESC_PSRAM_ENABLE)
#error "ESC can't support Flash and PSRAM at the same time."
#endif

#if (!defined(ESC_FLASH_ENABLE)) && (!defined(ESC_PSRAM_ENABLE))
#error "Please Specify the device type (Flash or PSRAM) that is attached to the ESC."
#endif

/* Private typedef ---------------------------------------------------*/
// #define ESC_INTERNAL_DEBUG

typedef enum{
    ESC_SPI_MODE = 0,                         /* CMD: 1 bit  ADDR: 1 bit  DATA: 1 bit */
    ESC_SPIQ_MODE = 1,                        /* CMD: 1 bit  ADDR: 4 bit  DATA: 4 bit */
    ESC_QPI_MODE = 2,                         /* CMD: 4 bit  ADDR: 4 bit  DATA: 4 bit */
    ESC_INVALID_MODE = 0xFFFFFFFF
} hal_esc_mode_t;


typedef enum{
    REG_QE = 0,                               /* quad enable */
    REG_WEL = 1,                              /* write enable latch */
    REG_BUSY = 2,
    REG_SUSPEND = 3
} esc_device_reg_type_t;

typedef enum{
    ESC_CLOCK_26MHz = 0,                      /* ESC frequency enum align with clock side */
    ESC_CLOCK_52MHz = 2,
    ESC_CLOCK_INVALID = 0xFF
} esc_clock_t;


#ifdef HAL_SLEEP_MANAGER_ENABLED
typedef enum{
    ESC_REG_MAC_IRQ = 0,
    ESC_REG_DIRECT_CTL = 1,
    ESC_REG_DLY_CTL2 = 2,
    ESC_REG_DLY_CTL3 = 3,
    ESC_REG_DLY_CTL4 = 4,
    ESC_REG_MAX_IDX = 5
} esc_backup_reg_t;
#endif /* HAL_SLEEP_MANAGER_ENABLED */

#ifdef ESC_FLASH_ENABLE
typedef struct{
    const char *device_name;
    const uint32_t jedec_id;

    const uint32_t page_size;
    const uint32_t page_number;

    const uint8_t read_cmd;                   /* ESC with flash support spi/spiq direct read */
    const uint8_t spiq_read_cmd;              /* CMD: 1 bit  ADDR: 4 bit  DATA: 4 bit */
    const uint8_t write_cmd;                  /* ESC with flash only support spi mac write */
    const uint8_t resume_cmd;                 /* flash device must support suspend & resume cmd */
    const uint8_t suspend_cmd;
    const uint8_t qe_bit;
    const uint8_t busy_bit;
    const uint8_t suspend_bit;

    const uint8_t erase_cmd[4];               /* ESC with flash support 4/32/64/total erase type */

    const uint32_t delay_ctl_52MHz[4];        /* ESC delay setting for 52MHz */
    const uint32_t delay_ctl_26MHz[4];        /* ESC delay setting for 26MHz */
} esc_device_info_t;


typedef struct{
    hal_esc_mode_t mode;
    const esc_device_info_t *device;
    esc_clock_t clock;
    bool busy;
    bool suspended;
    bool qe_bit;
    bool initialized;
    bool ahb_req_while_mac;
} esc_ctl_block_t;

#endif /* ESC_FLASH_ENABLE */

// GPRAM 4-byte alined access union
typedef union {
    uint32_t   u32;
    uint16_t   u16[2];
    uint8_t    u8[4];
} esc_uint;

typedef struct {
    hal_esc_mode_t DEVICE_MODE;
    uint32_t ESC_MAC_CTL;
    uint32_t ESC_DIRECT_CTL;
    uint32_t ESC_MISC_CTL1;
    uint32_t ESC_MISC_CTL2;
    uint32_t ESC_DLY_CTL_52MHz[4];
    uint32_t ESC_DLY_CTL_26MHz[4];
} esc_psram_setting;


/* Private macro ---------------------------------------------*/
#define ESC_GPRAM_SIZE     (128)
#define ESC_GPRAM_BASE     (ESC_BASE + 0x800)
#define ESC_MAC_EN         (0x00000100)
#define ESC_MAC_TRIG       (0x00010000)
#define ESC_MAC_WAIT_DONE  (0x00000001)
#define ESC_IDLE           (0x00000001)
#define ESC_DIRECT_BUSY    (0x40000000)

#define ESC_GENERIC_1_BIT_OFFSET      (1)
#define ESC_GENERIC_2_BIT_OFFSET      (2)
#define ESC_GENERIC_4_BIT_OFFSET      (4)
#define ESC_GENERIC_8_BIT_OFFSET      (8)
#define ESC_GENERIC_10_BIT_OFFSET    (10)
#define ESC_GENERIC_16_BIT_OFFSET    (16)
#define ESC_GENERIC_24_BIT_OFFSET    (24)
#define ESC_GENERIC_31_BIT_OFFSET    (31)

/* ESC generic mask definition */
#define ESC_GENERIC_0x1_MASK         (0x1)
#define ESC_GENERIC_0x0F_MASK        (0x0F)
#define ESC_GENERIC_0xF0_MASK        (0xF0)
#define ESC_GENERIC_0xFF_MASK        (0xFF)
#define ESC_GENERIC_0xF000_MASK      (0xF000)
#define ESC_GENERIC_0x00FF_MASK      (0x00FF)
#define ESC_GENERIC_0x0FFFFFFF_MASK  (0x0FFFFFFF)
#define ESC_GENERIC_0x000000FF_MASK  (0x000000FF)
#define ESC_GENERIC_0x0000FF00_MASK  (0x0000FF00)
#define ESC_GENERIC_0x00FF0000_MASK  (0x00FF0000)
#define ESC_GENERIC_0xFF000000_MASK  (0xFF000000)
#define ESC_GENERIC_0xFFFFFF00_MASK  (0xFFFFFF00)
#define ESC_GENERIC_SRAM_BANK_MASK   (0x05000000)

#define ESC_DIRECT_REG_DEFAULT       (0x03027000)
#define ESC_GPRAM_DATA               (ESC_BASE + 0x0800)

#define ESC_DIRECT_READ_CMD_BIT_OFFSET      (24)
#define ESC_DIRECT_WRITE_CMD_BIT_OFFSET     (16)
#define ESC_RD_DUMMY_BIT_OFFSET             (12)
#define ESC_WR_DUMMY_BIT_OFFSET             (8)
#define ESC_W_R_MODE_BIT_OFFSET             (4)
#define ESC_RD_DUMMY_EN_BIT_OFFSET          (1)
#define ESC_WR_DUMMY_EN_BIT_OFFSET          (0)

#ifdef MICRO_CHIP_23A1024
//device no ID
#define ESC_CMD_READ             (0x03)
#define ESC_CMD_WRITE            (0x02)

#define ESC_CMD_ENTER_QUAD_MODE  (0x3B)
#define ESC_CMD_EXIT_QUAD_MODE   (0xFF)
#define ESC_CMD_MODE_READ        (0x05)
#define ESC_CMD_MODE_WRITE       (0x01)
#endif


#ifdef ASP_CHIP_1604M_SQ_PSRAM
#define ESC_CMD_READ_ID          (0x9F)
#define ESC_CMD_READ             (0x03)
#define ESC_CMD_FAST_READ        (0x0B)
#define ESC_CMD_QUAD_READ        (0xEB)
#define ESC_CMD_WRAP_READ        (0x8B)
#define ESC_CMD_WRITE            (0x02)
#define ESC_CMD_WRITE_QUAD       (0x38)
#define ESC_CMD_WRAP_WRITE       (0x82)
#define ESC_CMD_MODE_READ        (0xB5)
#define ESC_CMD_MODE_WRITE       (0xB1)
#define ESC_CMD_ENTER_QUAD_MODE  (0x35)
#define ESC_CMD_EXIT_QUAD_MODE   (0xF5)
#define ESC_CMD_RESET_ENABLE     (0x66)
#define ESC_CMD_RESET            (0x99)
#define ESC_CMD_BURST_LEN        (0xC0)
#endif

#ifdef ESC_ASP6404L_SQ_PSRAM
#define ESC_CMD_READ_ID          (0x9F)
#define ESC_CMD_READ             (0x03)
#define ESC_CMD_FAST_READ        (0x0B)
#define ESC_CMD_QUAD_READ        (0xEB)
#define ESC_CMD_WRITE            (0x02)
#define ESC_CMD_WRITE_QUAD       (0x38)
#define ESC_CMD_ENTER_QUAD_MODE  (0x35)
#define ESC_CMD_EXIT_QUAD_MODE   (0xF5)
#define ESC_CMD_RESET_ENABLE     (0x66)
#define ESC_CMD_RESET            (0x99)
#define ESC_CMD_BURST_LEN        (0xC0)
#endif

/* flash erase length macro */
#define ESC_ERASE_4K_LENGTH          (0x00001000)
#define ESC_ERASE_32K_LENGTH         (0x00008000)
#define ESC_ERASE_64K_LENGTH         (0x00010000)


/* Macro of flash device command */
#define READ_JEDEC_ID_CMD            (0x9F)

#define WINBOND_MANUFACTURER_ID      (0xEF)

#define WB_READ_QE_CMD               (0x35)
#define WB_READ_WEL_CMD              (0x05)
#define WB_READ_BUSY_CMD             (0x05)
#define WB_READ_SUSPEND_CMD          (0x35)

#define WB_WRITE_QE_CMD              (0x31)
#define WB_WRITE_WEL_CMD             (0x06)

#define WB_POWER_DOWN_CMD            (0xB9)
#define WB_RELEASE_POWER_DOWN_CMD    (0xAB)

/* Private functions ---------------------------------------------------------*/
#define ESC_ReadReg8(addr)           *((volatile unsigned char *)(addr))
#define ESC_ReadReg32(addr)          *((volatile unsigned int *)(addr))
#define ESC_WriteReg32(addr, data)   *((volatile unsigned int *)(addr)) = (unsigned int)(data)

#define ESC_PARSE_MANU_ID(jedec_id)  ((jedec_id>>16) & 0xFF)

void esc_internal_init(void);
void reset_esc_register(void);
bool esc_set_freq_related_parameter(esc_clock_t freq);
void esc_mask_channel(void);
void esc_unmask_channel(void);

#ifdef ESC_FLASH_ENABLE
bool esc_judge_device(void);
void set_initial_state(bool state);
bool get_initial_state(void);
void esc_flash_return_ready(void);
#endif /* ESC_FLASH_ENABLE */

#endif /* HAL_ESC_MODULE_ENABLED */

#endif /* __HAL_ESC_INTERNAL_H__ */

