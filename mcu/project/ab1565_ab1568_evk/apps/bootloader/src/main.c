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

#include <string.h>

#include "bl_common.h"
#include "bl_fota.h"
#include "hal_uart.h"
#include "hal_flash.h"
#include "core_cm4.h"
#include "hal_emi.h"
#include "hal_clock_internal.h"
/*#include "hal_dcxo.h"*/
#include "hal_cache.h"
#include "hal_pmu.h"
#include "bsp_flash.h"
#include "hal_gpio.h"
#include "hal_usb_internal.h"
#ifdef MTK_SECURE_BOOT_ENABLE
#include "secure_boot.h"
#endif
#define BL_EXECUTION_VIEW_ADDRESS SYSRAM_BASE
#define BL_CACHE_ENABLE
#define BL_ENABLE_JTAG
#define PARSE_PARTITION_TABLE

#ifdef BL_CACHE_ENABLE


#ifdef MTK_LAYOUT_PARTITION_ENABLE
#include "memory_map.h"
#include "layout_partition.h"
#endif

extern uint32_t *_edata[];
extern uint32_t *nc_data_size[];
extern uint32_t *_s_nc_data[];
#ifdef HAL_FLASH_MODULE_ENABLED
extern int sfi_index;
extern uint8_t nor_id[4];
#endif

/**
* @brief       This function is to initialize cache controller.
* @param[in]   None.
* @return      None.
*/
void cache_init(void)
{
    hal_cache_region_t region, region_number;

    /* Max region number is 16 */
    hal_cache_region_config_t region_cfg_tbl[] = {
        /* cacheable address, cacheable size(both MUST be 4k bytes aligned) */
        /* bootloader execution at SYSRAM */
        {BL_EXECUTION_VIEW_ADDRESS, BL_LENGTH},
    };

    region_number = (hal_cache_region_t)(sizeof(region_cfg_tbl) / sizeof(region_cfg_tbl[0]));

    hal_cache_init();
    hal_cache_set_size(HAL_CACHE_SIZE_16KB);

    for (region = HAL_CACHE_REGION_0; region < region_number; region++) {
        hal_cache_region_config(region, &region_cfg_tbl[region]);
        hal_cache_region_enable(region);
    }

    for (; region < HAL_CACHE_REGION_MAX; region++) {
        hal_cache_region_disable(region);
    }

    hal_cache_enable();
}

/**
* @brief       This function is to de-initialize cache controller.
* @param[in]   None.
* @return      None.
*/
void cache_deinit(void)
{
    hal_cache_disable();
    hal_cache_deinit();
}
#endif

/* Placement at SYSRAM for SFC/EMI initialization.
   In XIP case, bl_print should NOT be called between
   hal_clock_set_pll_dcm_init and hal_emi_configure_advanced/custom_setSFIExt*/

#ifdef MTK_LAYOUT_PARTITION_ENABLE
const partition_t protect_partition_id[] = {PARTITION_BL, PARTITION_NVDM};
#endif
ATTR_TEXT_IN_TCM void bl_hardware_init()
{
  #ifdef BL_CACHE_ENABLE
    //cache_init();
  #endif

#ifdef BL_ENABLE_JTAG
    //hal_pinmux_set_function(0,5);
    //hal_pinmux_set_function(1,5);
#endif

#ifdef HAL_DCXO_MODULE_ENABLED
    hal_dcxo_init();
#endif
     
    /* To set DCXO frequency for hal_clock_fxo_is_26m */
    hal_clock_init();
    /* temp for clock risk */

    hal_uart_config_t uart_config;

    /* UART init */
    uart_config.baudrate = HAL_UART_BAUDRATE_115200;
    uart_config.parity = HAL_UART_PARITY_NONE;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    hal_uart_init(HAL_UART_0, &uart_config);

    /* Enable FPU. Set CP10 and CP11 Full Access.  bl_print_internal in keil uses FPU.*/
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));
    bl_print(LOG_DEBUG,"set CP10 and CP11 Full Access\r\n");

    /* init noncache rw section */
    //memcpy(_s_nc_data,_edata,(size_t)nc_data_size);
#if 0
    /* SW workaround for EOSC accuracy issue, need measure EOSC frequency before die temperature up.*/
    bool rtc_flag = false;
    extern bool rtc_query_32kless_cali(void);
    //rtc_flag = rtc_query_32kless_cali();
    //bl_print(LOG_DEBUG, "1 is rtc 32kless mode: %d\r\n", rtc_flag);
#endif
    /* print log */
    bl_set_debug_level(LOG_DEBUG);
    bl_print(LOG_DEBUG, "bl_uart_init\r\n");

    //bl_print(LOG_DEBUG, "hal_emi_configure\r\n");
    //hal_emi_configure();

    /* PLL init */
    //bl_print(LOG_DEBUG, "hal_clock_set_pll_dcm_init\r\n");
    hal_clock_set_pll_dcm_init();

    /* latch power key */
    extern void pmu_latch_power_key_for_bootloader();
    pmu_latch_power_key_for_bootloader();
    bl_print(LOG_DEBUG, "hal_latch_power_key\r\n");

    //hal_emi_configure_advanced();

    /* SFI init required for SFC clock change. */

    custom_setSFIExt();
#ifdef HAL_FLASH_MODULE_ENABLED
    bl_print(LOG_DEBUG, "flash ID =0x%x, 0x%x, 0x%x \r\n", nor_id[0], nor_id[1], nor_id[2]);
    bl_print(LOG_DEBUG, "sfi_index=%d\r\n", sfi_index);
#endif
    bl_print(LOG_DEBUG, "NOR_init\r\n");
    hal_flash_init();

#ifdef MTK_LAYOUT_PARTITION_ENABLE
    lp_init();
    lp_register_readonly_partitions(protect_partition_id, sizeof(protect_partition_id)/sizeof(partition_t));
#endif

    bl_print(LOG_DEBUG, "hf_fsys_ck freq=%d\r\n", hal_clock_get_freq_meter(_hf_fsys_ck, 100)/2);
    bl_print(LOG_DEBUG, "hf_fsfc_ck freq=%d\r\n", hal_clock_get_freq_meter(_hf_fsfc_ck, 100));
	//while(*(volatile uint32_t *)(ESC_BASE+10) != 0x23455456);

    /* SF STT and Disturbance Test*/
#ifdef __SERIAL_FLASH_STT_EN__
    extern void stt_main(void);
    stt_main();
#endif

    /* deinit brom usb */
    hal_usb_deinit_brom();

    // Clear crypto irq raised in BROM
    //NVIC_ClearPendingIRQ(CRYPTO_IRQn);

}

#ifdef PARSE_PARTITION_TABLE

typedef struct {
  uint32_t BinaryId;
  uint32_t PartitionId;
  uint32_t LoadAddressHigh;
  uint32_t LoadAddressLow;
  uint32_t BinaryLengthHigh;
  uint32_t BinaryLengthLow;
  uint32_t ExecutionAddress;
  uint32_t ReservedItem0;
  uint32_t ReservedItem1;
  uint32_t ReservedItem2;
  uint32_t ReservedItem3;
  uint32_t ReservedItem4;
} PartitionTableItem;

typedef struct {
  PartitionTableItem SecHdr1;
  PartitionTableItem SecHdr2;
  PartitionTableItem bootloader;
  PartitionTableItem CM4;
  PartitionTableItem DSP0;
  PartitionTableItem fotaReserve;
  PartitionTableItem NVDM;
  PartitionTableItem ROFS;
} PartitionTable;

#endif

void bl_start_user_code()
{
#if defined(MTK_SECURE_BOOT_ENABLE)
    sboot_status_t ret = SBOOT_STATUS_OK;
    uint32_t hdrAddr = bl_custom_header_start_address();
#endif
#ifdef PARSE_PARTITION_TABLE
    uint32_t targetAddr;
    PartitionTable pt;
    hal_flash_status_t flash_status = HAL_FLASH_STATUS_OK;
    int32_t fallback = 0;

    targetAddr = bl_custom_cm4_start_address();

    /* partition table is at the start of flash */
    flash_status = hal_flash_read(0, (uint8_t *)&pt, sizeof(PartitionTable));
    if (flash_status != HAL_FLASH_STATUS_OK) {
     fallback = -1;
    }
    if (pt.CM4.BinaryId != 0x3) {
     fallback = -2;
    }

    if (fallback == 0) {
     targetAddr = pt.CM4.LoadAddressLow;
    }
#if defined(MTK_SECURE_BOOT_ENABLE)
    if (pt.SecHdr2.BinaryId != 0x8)
    {
        fallback = -3;
    }
    else
    {
        hdrAddr = pt.SecHdr2.LoadAddressLow;
    }
#endif

    bl_print(LOG_DEBUG, "Partition(%d) %x\r\n", fallback, targetAddr);
#else
    uint32_t targetAddr = bl_custom_cm4_start_address();
#endif

#if defined(MTK_SECURE_BOOT_ENABLE)
    bl_print(LOG_DEBUG, "bl_custom_header_start_address = %x\r\n", hdrAddr);

    ret = sboot_secure_boot_check((uint8_t *)hdrAddr, NULL, SBOOT_IOTHDR_V2, 0);

    if (ret == SBOOT_STATUS_FAIL) {
        bl_print(LOG_DEBUG, "secure boot check failed. system halt (%x)\r\n", ret);
        while(1);
    } else if (ret == SBOOT_STATUS_NOT_ENABLE) {
        bl_print(LOG_DEBUG, "secure boot disabled\r\n");
    } else if(ret == SBOOT_STATUS_OK) {
        bl_print(LOG_DEBUG, "secure boot check pass\r\n");
    }
    else
    {
        bl_print(LOG_DEBUG, "secure boot check failed. system halt (%x)\r\n", ret);
        while(1);
    }
#endif
#ifdef BL_CACHE_ENABLE
        //cache_deinit();
#endif
    bl_print(LOG_DEBUG, "Jump to addr %x\r\n\r\n", targetAddr);
    JumpCmd(targetAddr);
}

int main()
{
   bl_hardware_init();

#ifdef BL_FOTA_ENABLE
     bl_fota_process();
#endif

    bl_start_user_code();

    return 0;
}

