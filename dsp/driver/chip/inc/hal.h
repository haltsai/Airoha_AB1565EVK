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
 
#ifndef __HAL_H__
#define __HAL_H__

/**
 * @addtogroup HAL
 * @{
 * This section introduces the HAL driver APIs including terms and acronyms, supported features, software architecture,
 * details on how to use this driver, HAL function groups, enums, structures and functions.
 * @section HAL_Overview_1_Chapter    Terms and acronyms
 *
 * |Acronyms                         |Definition                                                           |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b EINT                 | External interrupt
 * |\b GDMA                 | General direct memory access
 * |\b GPIO                 | General purpose Input Output
 * |\b GPT                  | General purpose timer
 * |\b I2C                  | Inter-integrated circuit
 * |\b I2S                  | Inter-integrated sound
 * |\b MPU                  | Memory protect unit
 * |\b SPI                  | Serial Peripheral Interface
 * |\b UART                 | Universal asynchronous receiver/transmitter
 * |\b USB                  | Universal Serial Bus
 * |\b DCM                  | Dynamic Clock Management
 *
 * @section HAL_Overview_2_Chapter    Overview of HAL drivers
 *
 * - \b Introduction \n
 *   Airoha SDK hardware abstraction layer (HAL) driver is embedded software ensuring maximized portability across IoT portfolio designed for Airoha IoT SDK development platform.\n
 *   The HAL driver interface is common for DSP0 and DSP1, because DSP0 and DSP1 with same type of processor and share same peripheral hardware.
 *   The HAL driver includes:\n
 *   - Drivers for the peripheral modules, such as UART and I2C;\n
 *   - Drivers for the system modules, such as CACHE, MPU and FLASH;\n
 *   - APIs of the modules.\n
 *
 *   The HAL driver has both standard and advanced features and is easy to use. It makes the upper hardware abstraction layer portable.\n
 *   The HAL layer provides a generic set of APIs to interact with the upper layers, such as application, middleware and the OS. The APIs are defined interfaces for Airoha IoT portfolio, so the upper layer written with the HAL API is portable across the Airoha IoT platforms.\n
 *   The HAL driver covers rich modules, including all peripheral modules, such as UART, I2C and SPI. It also includes number of system modules, such as CACHE, MPU, FLASH and DMA.\n
 *   The HAL complies with the general naming convention and common coding style. Each module of the HAL has its distinctive folder structure along with supporting examples.
 * - \b HAL \b driver \b architecture \n
 * The HAL driver architecture is shown below:\n
 * @image html hal_overview_driver_architecture.png
 * - \b HAL \b folder \b structure \b and \b file \b inclusion \b model \n
 *  - \b HAL \b folder \b structure \n
 *    The HAL module folder structure is shown below:
 * @image html hal_overview_folder_structure.png
 *    - Driver: All common driver files, such as board driver, HAL driver.\n
 *    - Driver/board: The driver files that are associated with the board, such as hw_resource_assignment  or sct key.\n
 *    - Driver/chip/inc: The public header files of the HAL.\n
 *    - Driver/chip/mtxxxx: The driver files that are specific to the chipset, such as UART, I2C, GPT, PWM, WDT, etc.\n
 *
 *  - \b File \b inclusion \b model \n
 * @image html hal_overview_file_inclusion_model.png
 *
 * - \b HAL \b driver \b rules \n
 *  - \b HAL \b file  \b naming \b convention \n
 *       The HAL public header files are named as hal_{feature}.h(e.g., hal_adc.h.)\n
 *       All HAL drivers are in driver/chip/mtxxxx/inc and driver/chip/mtxxxx/src,
 *       they are named as hal_{feature}.c or hal_{feature}_{sub_feature}.c,
 *       (e.g., hal_cache.c and hal_cache_internal.c)\n
 *  - \b HAL \b public \b API \b naming \b convention \n
 *       |                       |naming convention                                                             |
 *       |-----------------------|------------------------------------------------------------------------|
 *       |function names         | hal_{feature}_{sub_feature}(), such as #hal_nvic_init()
 *       |enum names             | Type name:hal_{feature}_{sub_feature}_t, such as #hal_nvic_status_t. \n Enum member names:HAL_{FEATURE}_{SUB_FEATURE}, such as #HAL_NVIC_STATUS_OK.
 *       |struct name            | Type name:hal_{feature}_{sub_feature}_t,such as #hal_eint_config_t.\n Struct member names must be lowercase.
 *       |macro names            | HAL_{FEATURE}_{SUB_FEATURE}, such as #HAL_NVIC_QUERY_EXCEPTION_NUMBER
 *       |function pointer names | hal_{feature}_{sub_feature}_t(), such as void (*#hal_nvic_isr_t)().\n
 *
 * - \b HAL \b configuration \n
 *  - \b hal_feature_config.h \n
 *      Every project should include hal_feature_config.h for HAL configuration.\n
 *      If certain HAL modules are not used, they can be removed from the feature options by undefining the macro, such as undefine the option of HAL_CACHE_MODULE_ENABLED.\n
 *      For customized parameter settings.\n
 * \n
 * \n
 * @section HAL_Overview_3_Chapter    HAL Driver Model
 * Some drivers can operate in two modes: polling and interrupt.
 * The UART HAL driver, for example, can operate in polling and interrupt modes during data communication. The polling mode is suitable for read and write operations of a small amount of data in low frequency. The interrupt mode is suitable for read and write operations of small amount of data in high frequency. In UART DMA mode, for example, an interrupt is triggered when the DMA is complete.
 * - \b Polling \b mode \b architecture \n
 *   Most of the driver modules support the polling mode, which is the basic feature in HAL.\n
 *   For example, call the #hal_gpt_delay_ms() API to apply a millisecond time delay in the GPT. The GPT HAL driver will poll the GPT hardware if the time delay is reached.\n
 *   The polling mode driver architecture is shown below:
 *   @image html hal_overview_driver_polling_architecture.png
 *
 * - \b Interrupt \b mode \b architecture \n
 *   To improve the portability, the HAL driver hides the OS dependence by preventing the upper layer to call the OS level services directly. The HAL drivers uses an interrupt handler to provide the upper layer with a callback mechanism.
 *
 *   To use the interrupt handler of a driver module, a callback must be registered with the interrupt handler. When the interrupt occurs, the callback is invoked in Cortex-M4 handler mode.
 *   Call the #hal_gpt_sw_start_timer_ms() API to start a timer. When the timer expires, the GPT hardware issues an interrupt. The GPT HAL driver then invokes the user callback in the interrupt handler to notify that the timer has expired.
 *   The interrupt mode driver architecture is shown in the following image:
 *   @image html hal_overview_interrupt_mode_architecture.png
 *
 *   All HAL APIs are thread-safe and available for ISR. Developers can call HAL APIs in any FreeRTOS task or in an ISR.\n
 *   However, some hardware has limited resources, and the corresponding APIs return an "ERROR_BUSY" status when there is a resource conflict due to the re-entrance. Developers should always check the return values from HAL APIs.\n
 * @section HAL_Overview_4_Chapter    The Architecture Of DSP Platform
 *   "Processors have traditionally been extremely difficult to design and modify. Therefore, most systems contain rigid processors that were designed and verified once for general-purpose use and then embedded into multiple applications over time. Because these processors are general-purpose designs, their suitability to any particular application is less than ideal. Although it would be preferable to have a processor specifically designed to execute a particular application¡¯s code better (for example, to run faster, or consume less power, or cost less), this is rarely possible because of the difficulty; the time, cost, and risk of modifying an existing processor or developing a new processor is very high."-Reference from "Xtensa Instruction Set Architesture (ISA).pdf"\n
 *   So we need to add some DSP processors to our platform. For example, On the platform of AB155x, we used the DSP of HiFi Mini designed by Xtensa.\n
 *   On AB155x platform there are two DSP processors and share all of the external hardware resources including the bus system and peripherals. The architecture is shown below.\n
 *   @image html hal_overview_dsp_platform_architecture.png
 * 
 * @section HAL_Overview_5_Chapter    The convention of cross core memory access
 *  Every core have its own sleep and DCM mechanism. \n
 *  Because DSP0 I/DRAM and DSP1 I/DRAM locate in the core, if DSP0 or DSP1 enter sleep or enter DCM mode, other cores or DMA will can not access DSP0 and DSP1.\n
 *  So if user want to do DSP I/DRAM memory access cross cores, need to do sleep lock for the target core.
 *  @image html hal_overview_cross_core_memory_access.png
 *  Note:\n 
 *       1. For customer and upper layer users, CM4 should not access DSP0 and DSP1 I/DRAM.
 *       2. When user call hal_sleep_manager_lock_sleep() to do sleep lock cross cores, must guarantee the target core had been boot up. \n
 *         Because the fucntion means the source core will send event via CCNI to the target core and wait for the target core lock sleep by itself done.\n
 *  The pseudocode of CPU to do memory access cross cores:
 * @code
 *      //DSP0 want to access DSP1 DRAM:
 *      hal_sleep_manager_lock_sleep(SLEEP_LOCK_DSP1_CROSS_MEMORY_ACCESS);
 *      // please add your code of DSP0 access DSP1 DRAM to here!!!
 *      hal_sleep_manager_unlock_sleep(SLEEP_LOCK_DSP1_CROSS_MEMORY_ACCESS);
 * @endcode
 *  The pseudocode of DMA to do memory access cross cores:
 * @code
 *  //DSP0 start DMA to access DSP1 DRAM:
 *  // User should lock DSP1 sleep before start DMA
 *  // source_address or destination_address is DSP1 DRAM.
 *       hal_sw_gdma_status_t status;
 *       temp_dma_cfg.source_address = source_address;
 *       temp_dma_cfg.destination_address = destination_address;
 *       temp_dma_cfg.length = dma_length;
 *       temp_dma_cfg.func = user_dma_transfer_done_callback;
 *       temp_dma_cfg.argument = NULL;
 *       temp_dma_cfg.h_size = HAL_SW_DMA_WORD;
 *       temp_dma_cfg.dma_type = HAL_SW_DMA_NORMAL_MODE;
 *       hal_sleep_manager_lock_sleep(SLEEP_LOCK_DSP1_CROSS_MEMORY_ACCESS);
 *       status = hal_sw_gdma_start(&temp_dma_cfg);
 *       if (status != HAL_SW_DMA_STATUS_OK) {
 *           // Error handling.
 *       }
 * 
 *  // this function is the DMA callback, user should do DSP1 sleep unlock after DMA copy done.
 *      void user_dma_transfer_done_callback()
 *      {
 *          // now DMA copy done!!!
 *          hal_sleep_manager_unlock_sleep(SLEEP_LOCK_DSP1_CROSS_MEMORY_ACCESS);
 *      }
 * @endcode
 * @}
 */


#include "hal_feature_config.h"
#include "hal_log.h"

/*****************************************************************************
* module header file include
*****************************************************************************/
#ifdef HAL_ACCDET_MODULE_ENABLED
#include "hal_accdet.h"
#endif

#ifdef HAL_ADC_MODULE_ENABLED
#include "hal_adc.h"
#endif

#ifdef HAL_AES_MODULE_ENABLED
#include "hal_aes.h"
#endif

#ifdef HAL_DAC_MODULE_ENABLED
#include "hal_dac.h"
#endif

#ifdef HAL_DES_MODULE_ENABLED
#include "hal_des.h"
#endif

#ifdef HAL_AUDIO_MODULE_ENABLED
#include "hal_audio.h"
#endif

#ifdef HAL_CACHE_MODULE_ENABLED
#include "hal_cache.h"
#endif

#ifdef HAL_CLOCK_MODULE_ENABLED
#include "hal_clock.h"
#endif

#ifdef HAL_DVFS_MODULE_ENABLED
#include "hal_dvfs.h"
#endif

#ifdef HAL_DES_MODULE_ENABLED
#include "hal_des.h"
#endif

#ifdef HAL_EINT_MODULE_ENABLED
#include "hal_eint.h"
#endif

#ifdef HAL_FLASH_MODULE_ENABLED
#include "hal_flash.h"
#endif

#ifdef HAL_SW_DMA_MODULE_ENABLED
#include "hal_sw_dma.h"
#endif

#ifdef HAL_GPC_MODULE_ENABLED
#include "hal_gpc.h"
#endif

#ifdef HAL_GPIO_MODULE_ENABLED
#include "hal_gpio.h"
#endif

#ifdef HAL_GPT_MODULE_ENABLED
#include "hal_gpt.h"
#endif

#ifdef HAL_I2C_MASTER_MODULE_ENABLED
#include "hal_i2c_master.h"
#endif

#ifdef HAL_I2S_MODULE_ENABLED
#include "hal_i2s.h"
#endif

#ifdef HAL_IRRX_MODULE_ENABLED
#include "hal_irrx.h"
#endif

#ifdef HAL_IRTX_MODULE_ENABLED
#include "hal_irtx.h"
#endif

#ifdef HAL_ISINK_MODULE_ENABLED
#include "hal_isink.h"
#endif

#ifdef HAL_KEYPAD_MODULE_ENABLED
#include "hal_keypad.h"
#endif

#ifdef HAL_MD5_MODULE_ENABLED
#include "hal_md5.h"
#endif

#ifdef HAL_MPU_MODULE_ENABLED
#include "hal_mpu.h"
#endif

#ifdef HAL_NVIC_MODULE_ENABLED
#include "hal_nvic.h"
#endif

#ifdef HAL_PWM_MODULE_ENABLED
#include "hal_pwm.h"
#endif

#ifdef HAL_RTC_MODULE_ENABLED
#include "hal_rtc.h"
#endif

#ifdef HAL_SD_MODULE_ENABLED
#include "hal_sd.h"
#endif

#ifdef HAL_SDIO_MODULE_ENABLED
#include "hal_sdio.h"
#endif

#ifdef HAL_SDIO_SLAVE_MODULE_ENABLED
#include "hal_sdio_slave.h"
#endif

#ifdef HAL_SHA_MODULE_ENABLED
#include "hal_sha.h"
#endif

#ifdef HAL_SPI_MASTER_MODULE_ENABLED
#include "hal_spi_master.h"
#endif

#ifdef HAL_SPI_SLAVE_MODULE_ENABLED
#include "hal_spi_slave.h"
#endif

#ifdef HAL_TRNG_MODULE_ENABLED
#include "hal_trng.h"
#endif

#ifdef HAL_UART_MODULE_ENABLED
#include "hal_uart.h"
#endif

#ifdef HAL_USB_MODULE_ENABLED
#include "hal_usb.h"
#endif

#ifdef HAL_WDT_MODULE_ENABLED
#include "hal_wdt.h"
#endif

#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_platform.h"
#include "hal_spm.h"
#endif

#ifdef HAL_G2D_MODULE_ENABLED
#include "hal_g2d.h"
#endif

#ifdef HAL_PMU_MODULE_ENABLED
#include "hal_pmu.h"
#endif

#ifdef HAL_TIME_CHECK_ENABLED
#include "hal_time_check.h"
#endif

#ifdef HAL_CCNI_MODULE_ENABLED
#include "hal_ccni.h"
#endif

#ifdef HAL_HW_SEMAPHORE_MODULE_ENABLED
#include "hal_hw_semaphore.h"
#endif

#ifdef HAL_TIME_CHECK_ENABLED
#include "hal_time_check.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* __HAL_H__ */

