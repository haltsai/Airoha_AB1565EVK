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
/**
 * File: main.c
 *
 * Description: The file contains the main function entry.
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "task_def.h"

/* HAL includes. */
#include "hal.h"
#include "sys_init.h"
#include "syslog.h"
#include "memory_map.h"
#include "apps_init.h"
#include "app_home_screen_idle_activity.h"
#include "string.h"
#include "hal_resource_assignment.h"

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE
#include "systemhang_tracer.h"
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE */

#ifdef MTK_MEMORY_MONITOR_ENABLE
#include "memory_monitor.h"
#endif /* MTK_MEMORY_MONITOR_ENABLE */

#ifdef MTK_FOTA_VIA_RACE_CMD
#include "fota.h"
#include "fota_flash.h"
#endif

#ifdef MTK_RACE_CMD_ENABLE
#include "race_xport.h"
#include "race_bt.h"
#include "race_fota.h"
#include "race_app_race_event_hdl.h"
#include "race_app_bt_event_hdl.h"
#endif

#include "exception_handler.h"

#include "bt_app_common.h"
#include "bt_power_on_config.h"

#include "nvdm_config_factory_reset.h"

#ifdef MTK_VA_MODEL_MANAGER_ENABLE
#include "va_model_manager.h"
#endif /* MTK_VA_MODEL_MANAGER_ENABLE */

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
#include "audio_source_control.h"
#endif

#if( configUSE_MALLOC_FAILED_HOOK == 1 )
void vApplicationMallocFailedHook()
{
    configASSERT(0);
}
#endif

/* Create the log control block for different modules.
 * Please refer to the log dev guide under /doc folder for more details.
 */
log_create_module(main, PRINT_LEVEL_INFO);
log_create_module(main_c, PRINT_LEVEL_INFO);

#define mainCHECK_DELAY ( ( portTickType ) 1000 / portTICK_RATE_MS )

/**
 * @brief     Idle task hook.
 */
void vApplicationIdleHook(void)
{
#ifdef MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE
    log_offline_dump_idle_hook();
#endif
#ifdef MTK_NVDM_ENABLE
    log_save_filter();
#endif

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE
    systemhang_set_safe_duration(SYSTEMHANG_USER_CONFIG_COUNT_TOTAL - 1, 60 * 5);

#if (configUSE_TICKLESS_IDLE != 2)
    /* If tickless mode is not enabled, sometimes the idle task will run more than 1 minute. */
    /* For avoiding the wdt timeout in this case, feed WDT in every idle task's loop. */
    systemhang_wdt_feed_in_task_switch();
#endif /* configUSE_TICKLESS_IDLE != 2 */
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE */
    app_home_screen_check_power_off_and_reboot();
}

#ifdef MTK_MEMORY_MONITOR_ENABLE
int memorymonitor_memory_check(void)
{
    return 0;
}
#endif /* MTK_MEMORY_MONITOR_ENABLE */

/**
 * @brief     This function is to disable the part of log filters.
 */
static void log_set_filters_default_disabled(void)
{
    log_config_print_switch(main_c, DEBUG_LOG_OFF);
    /* ATCI log. */
    log_config_print_switch(atci_charger, DEBUG_LOG_OFF);
    //log_config_print_switch(atci, DEBUG_LOG_OFF);

    log_config_print_switch(gain_value_mapping, DEBUG_LOG_OFF);

    /* BT log. */
    log_config_print_switch(BT_GATTS, DEBUG_LOG_OFF);
#ifdef MTK_BLE_BAS
    log_config_print_switch(BLE_BAS, DEBUG_LOG_OFF);
#endif
#ifdef MTK_BLE_IAS
    log_config_print_switch(BLE_IAS, DEBUG_LOG_OFF);
#endif
    log_config_print_switch(BTHCI, DEBUG_LOG_OFF);
    log_config_print_switch(BTL2CAP, DEBUG_LOG_OFF);
    log_config_print_level(BTRFCOMM, PRINT_LEVEL_INFO);
    /*log_config_print_switch(BTSPP, DEBUG_LOG_OFF);*/
}


extern void reserved_nvdm_item_list_check(void);

/**
 * @brief     Main function.
 *
 * @details   This function will be used to initialize the system and create tasks.
 */
int main(void)
{
#ifdef REBOOT_AFTER_EXCEPTION
    exception_dump_config(DISABLE_WHILELOOP_MAGIC);
#endif

    /* Do system initialization, eg: hardware, nvdm. */
    system_init();
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
    extern const char BSP_DUAL_CHIP_TRIGGER_POWER_KEY_PIN;
    hal_pinmux_set_function(BSP_DUAL_CHIP_TRIGGER_POWER_KEY_PIN, 0);
    hal_gpio_set_direction(BSP_DUAL_CHIP_TRIGGER_POWER_KEY_PIN, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(BSP_DUAL_CHIP_TRIGGER_POWER_KEY_PIN, HAL_GPIO_DATA_HIGH);
    hal_gpt_delay_ms(2000);
    hal_gpio_set_output(BSP_DUAL_CHIP_TRIGGER_POWER_KEY_PIN, HAL_GPIO_DATA_LOW);
#endif
    /* Check if there's a factory reset and do nvdm clear. */
    factory_rst_reserved_nvdm_item_list_check();
    /* FOTA init must be put just after system_init(). */
#ifdef MTK_FOTA_VIA_RACE_CMD
    fota_init_flash();
#if FOTA_STORE_IN_EXTERNAL_FLASH
    fota_flash_config_fota_partition_in_external_flash(FOTA_EXT_RESERVED_BASE_DEFAULT,
                                                       FOTA_EXT_RESERVED_LENGTH_DEFAULT);
#endif
#endif
#ifdef MTK_FOTA_ENABLE
    reserved_nvdm_item_list_check();
#endif

    bt_power_on_config_init();

#ifdef MTK_RACE_CMD_ENABLE
    race_init();
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
    audio_source_control_init();
#endif
    race_app_init();
#ifdef MTK_FOTA_VIA_RACE_CMD
    race_fota_set_transmit_interval(RACE_FOTA_DEFAULT_SPP_TRANSMIT_INTERVAL_IN_MS,
                                    RACE_FOTA_DEFAULT_BLE_TRANSMIT_INTERVAL_IN_MS);

#endif
#endif


#ifdef MTK_VA_MODEL_MANAGER_ENABLE
    /**
     * @brief Add the code for va model manager for switch language model.
     */
    va_model_manager_partition_info_t info[1] = {
        {LM_BASE, LM_LENGTH}
    };
    va_model_manager_init(info, 1, ROM_BASE);
#endif /* MTK_VA_MODEL_MANAGER_ENABLE */

#ifdef HAL_TIME_CHECK_ENABLED
    extern exception_config_mode_t exception_config_mode;
    if (exception_config_mode.exception_mode_t.mask_irq_check_assert) {
        hal_time_check_enable();
    }
    memcpy((uint8_t *)(HW_SYSRAM_PRIVATE_MEMORY_EXCEPTION_START + HW_SYSRAM_PRIVATE_MEMORY_EXCEPTION_LEN - sizeof(exception_config_mode_t)), &exception_config_mode, sizeof(exception_config_mode_t));
#endif
    /* Create tasks. */
    task_def_init();
    task_def_create();

    /* Set some log filters default disabled. */
    log_set_filters_default_disabled();

    bt_app_common_init();

    apps_init();

    /* Call this function to indicate the system initialize done. */
    hal_core_status_write(HAL_CORE_CM4, HAL_CORE_ACTIVE);

    /* Start the scheduler. */
    vTaskStartScheduler();
    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for (;;);
}

