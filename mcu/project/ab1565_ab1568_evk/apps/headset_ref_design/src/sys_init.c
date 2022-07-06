/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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
/**
 * File: sys_init.c
 *
 * Description: The file is used for initialize the system, include hardware, logging port, system clock, etc.
 *
 */

#include <string.h>

/* HAL includes. */
#include "hal.h"
#include "syslog.h"
#include "bsp_gpio_ept_config.h"
#include "memory_attribute.h"
#include "hal_resource_assignment.h"
#include "dcxo_capid.h"
#include "exception_handler.h"
#ifdef MTK_NVDM_ENABLE
#include "nvdm.h"
#endif
#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE
#include "systemhang_tracer.h"
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE */
#ifdef MTK_BOOTREASON_CHECK_ENABLE
#include "bootreason_check.h"
#endif /* MTK_BOOTREASON_CHECK_ENABLE */
#ifdef MTK_MINIDUMP_ENABLE
#include "offline_dump.h"
#endif /* MTK_MINIDUMP_ENABLE */
#include "serial_port_assignment.h"
#include "verno.h"
#ifdef MTK_BATTERY_MANAGEMENT_ENABLE
#include "battery_management.h"
#endif
#if defined(AB1565)
#include "hal_pmu_cali_2565.h"
#include "hal_pmu_charger_2565.h"
#endif
/* Set system clock to 26M Hz. */
#ifdef MTK_SYSTEM_CLOCK_26M
static const uint32_t target_freq = 26000;
#else
/* Set system clock to 52M Hz. */
#ifdef MTK_SYSTEM_CLOCK_52M
static const uint32_t target_freq = 52000;
#else
/* Set system clock to 78M Hz. */
#ifdef MTK_SYSTEM_CLOCK_78M
static const uint32_t target_freq = 78000;
#else
/* Set system clock to 104M Hz. */
#ifdef MTK_SYSTEM_CLOCK_104M
static const uint32_t target_freq = 104000;
#else
/* Set system clock to 156M Hz. */
#ifdef MTK_SYSTEM_CLOCK_156M
static const uint32_t target_freq = 156000;
#else
/* Set system clock to 208M Hz. */
static const uint32_t target_freq = 208000;
#endif
#endif
#endif
#endif
#endif

/**
 * @brief       This function is to config system clock.
 * @param[in]   None.
 * @return      None.
 */
static void SystemClock_Config(void)
{
#ifndef FPGA_ENV
    hal_clock_init();
#endif
    /*#ifdef HAL_DVFS_MODULE_ENABLED
        hal_dvfs_init();
        hal_dvfs_target_cpu_frequency(target_freq, HAL_DVFS_FREQ_RELATION_H);
    #endif*/
}

/**
 * @brief       This function is to initialize cache controller.
 * @param[in]   None.
 * @return      None.
 */
static void cache_init(void)
{
    hal_cache_region_t region, region_number;

    /* Max region number is 16. */
    hal_cache_region_config_t region_cfg_tbl[] = {
        /* Cacheable address, cacheable size(both MUST be 4k bytes aligned). */
        {CM4_BASE, CM4_LENGTH},
        /* Virtual sysram. */
        {VSYSRAM_BASE, VSYSRAM_LENGTH}
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
 * @brief       Caculate actual bit value of region size.
 * @param[in]   region_size: actual region size.
 * @return      Corresponding bit value of region size for MPU setting.
 */
static uint32_t caculate_mpu_region_size(uint32_t region_size)
{
    uint32_t count;

    if (region_size < 32) {
        return 0;
    }
    for (count = 0; ((region_size  & 0x80000000) == 0); count++, region_size  <<= 1);
    return 30 - count;
}

/**
 * @brief       This function is to initialize MPU.
 * @param[in]   None.
 * @return      None.
 */
ATTR_TEXT_IN_SYSRAM void mpu_init(void)
{
    hal_mpu_region_t region, region_number;
    hal_mpu_region_config_t region_config;
    typedef struct {
        uint32_t mpu_region_base_address;/**< MPU region start address. */
        uint32_t mpu_region_end_address;/**< MPU region end address. */
        hal_mpu_access_permission_t mpu_region_access_permission;/**< MPU region access permission. */
        uint8_t mpu_subregion_mask;/**< MPU sub region mask. */
        bool mpu_xn;/**< XN attribute of MPU, if set TRUE, execution of an instruction fetched from the corresponding region is not permitted. */
    } mpu_region_information_t;

    /* TCM: VECTOR TABLE + CODE + RO DATA. */
    extern uint32_t Image$$TCM$$RO$$Base;
    extern uint32_t Image$$TCM$$RO$$Limit;
    /* SYSRAM: CODE+RO DATA. */
    extern uint32_t Image$$CACHED_SYSRAM_TEXT$$Base;
    extern uint32_t Image$$CACHED_SYSRAM_TEXT$$Limit;
    /* STACK END. */
    extern unsigned int Image$$STACK$$ZI$$Base[];

    /* MAX region number is 8. */
    mpu_region_information_t region_information[] = {
        /* mpu_region_start_address, mpu_region_end_address, mpu_region_access_permission, mpu_subregion_mask, mpu_xn. */
        {(uint32_t) ROM_BASE, (uint32_t)(ROM_BASE + ROM_LENGTH), HAL_MPU_READONLY, 0x0, FALSE}, //Set FLASH region as RO
        {(uint32_t) &Image$$TCM$$RO$$Base, (uint32_t) &Image$$TCM$$RO$$Limit, HAL_MPU_READONLY, 0x0, FALSE},//Vector table+TCM code+TCM rodata
        {(uint32_t) &Image$$CACHED_SYSRAM_TEXT$$Base, (uint32_t) &Image$$CACHED_SYSRAM_TEXT$$Limit, HAL_MPU_READONLY, 0x0, FALSE}, //SYSRAM code+SYSRAM rodata
        {(uint32_t) &Image$$CACHED_SYSRAM_TEXT$$Base - VRAM_BASE, (uint32_t) &Image$$CACHED_SYSRAM_TEXT$$Limit - VRAM_BASE, HAL_MPU_NO_ACCESS, 0x0, TRUE}, //Virtual memory
        {(uint32_t) &Image$$STACK$$ZI$$Base, (uint32_t) &Image$$STACK$$ZI$$Base + 32, HAL_MPU_READONLY, 0x0, TRUE}, //Stack end check for stack overflow
        {(uint32_t) 0, (uint32_t) 0x400000, HAL_MPU_NO_ACCESS, 0x0, TRUE} //Set EMI address range as no access
    };

    hal_mpu_config_t mpu_config = {
        /* PRIVDEFENA, HFNMIENA. */
        TRUE, TRUE
    };

    region_number = (hal_mpu_region_t)(sizeof(region_information) / sizeof(region_information[0]));

    hal_mpu_init(&mpu_config);
    for (region = HAL_MPU_REGION_0; region < region_number; region++) {
        /* Updata region information to be configured. */
        region_config.mpu_region_address = region_information[region].mpu_region_base_address;
        region_config.mpu_region_size = (hal_mpu_region_size_t) caculate_mpu_region_size(region_information[region].mpu_region_end_address - region_information[region].mpu_region_base_address);
        region_config.mpu_region_access_permission = region_information[region].mpu_region_access_permission;
        region_config.mpu_subregion_mask = region_information[region].mpu_subregion_mask;
        region_config.mpu_xn = region_information[region].mpu_xn;

        hal_mpu_region_configure(region, &region_config);
        hal_mpu_region_enable(region);
    }
    /* Make sure unused regions are disabled. */
    for (; region < HAL_MPU_REGION_MAX; region++) {
        hal_mpu_region_disable(region);
    }
    hal_mpu_enable();
}


/**
 * @brief       This function is to setup system hardware, such as cache init, uart init etc.
 * @param[in]   None.
 * @return      None.
 */
static void prvSetupHardware(void)
{
    /* Log service pre-initialize. */
    log_uart_init(CONFIG_SYSLOG_INIT_STAGE_PORT, CONFIG_SYSLOG_BAUDRATE);

    /* Sleep module initialize. */
    hal_sleep_manager_init();

    /* Cache module initialize. */
    cache_init();

    /* MPU module initialize. */
    mpu_init();

    /* Flash module initialize. */
    hal_flash_init();

    /* NVIC module initialize. */
    hal_nvic_init();

    /* CCNI module initialize. */
    hal_ccni_init();

    /* RTC module initialize. */
    hal_rtc_init();
}

/**
 * @brief       This function is to do system initialization, eg: system clock, hardware and logging port.
 * @param[in]   None.
 * @return      None.
 */
void system_init(void)
{
    syslog_port_type_t syslog_type;
    uint32_t           syslog_idx;

    /* Init private memory at the beginning of the system initialization. */
    private_memory_init();

    /* Call this function to indicate the system initialize is ongoing. */
    hal_core_status_write(HAL_CORE_CM4, HAL_CORE_INIT);

    /* SystemClock Config. */
    SystemClock_Config();
    SystemCoreClockUpdate();

    /* bsp_ept_gpio_setting_init() under driver/board/abxxxx_evk/ept will initialize the GPIO settings
     * generated by easy pinmux tool (ept). ept_*.c and ept*.h are the ept files and will be used by
     * bsp_ept_gpio_setting_init() for GPIO pinumux setup.
     */
    bsp_ept_gpio_setting_init();

    /* Configure the hardware ready to run the project. */
    prvSetupHardware();

#ifdef MTK_NVDM_ENABLE
    /* Nvdm initialize. */
    nvdm_init();
#endif
#if defined(AB1565)
    otp_calibration();
    mpk_init();
    pmu_bat_init();
#endif
#ifdef HAL_DVFS_MODULE_ENABLED
    hal_dvfs_init();
    hal_dvfs_target_cpu_frequency(target_freq, HAL_DVFS_FREQ_RELATION_H);

#endif

    /* Dcxo module initialize. */
    dcxo_load_capid();
    hal_dcxo_init();

#ifdef MTK_BATTERY_MANAGEMENT_ENABLE
    /* Initialize the battery function. */
    battery_management_init();
#endif

    /* Log init. */
    GET_SERIAL_PORT_TYPE_INDEX_FROM_PORT_DEV(CONFIG_SYSLOG_RUNNING_STAGE_PORT, syslog_type, syslog_idx);
    log_init(syslog_type, syslog_idx);
    log_set_filter();

    /* System initialize done log. */
    LOG_I(common, "[CM4]system initialize done[%s]", build_date_time_str);
    LOG_I(common, "FirmWare Version: %s", MTK_FW_VERSION);

#ifdef MTK_NVDM_ENABLE
    exception_dump_config_init();
#endif

#ifdef MTK_BOOTREASON_CHECK_ENABLE
    /* Bootreason service initialize. */
    bootreason_init();
#endif /* MTK_BOOTREASON_CHECK_ENABLE */

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE
    /* System hang tracer service initialize. */
    systemhang_tracer_init(15);
#else
    /* Bootloader will config wdt reset mode for 9s,So,we need to disable wdt */
    hal_wdt_disable(HAL_WDT_DISABLE_MAGIC);
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE */
}


