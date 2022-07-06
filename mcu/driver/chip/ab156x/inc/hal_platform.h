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

#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__


#include "hal_define.h"
#include "hal_feature_config.h"
#include "ab156x.h"
#include "memory_map.h"
#include "hal_core_status.h"

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
* Defines for module subfeatures.
* All the subfeatures described below are mandatory for the driver operation. No change is recommended.
*****************************************************************************/
#ifdef HAL_CLOCK_MODULE_ENABLED
#define HAL_CLOCK_METER_ENABLE           /*Enable clock meter feature */
#endif
#ifdef HAL_DVFS_MODULE_ENABLED
#define HAL_DVFS_416M_SOURCE             /*Enable 416Mhz / 208Mhz clock */
#define HAL_DVFS_LOCK_CTRL_ENABLED       /*Enable dvfs lock control relative api*/
#endif
#ifdef HAL_CACHE_MODULE_ENABLED
#define HAL_CACHE_WITH_REMAP_FEATURE     /* Enable CACHE setting with remap feature. */
#define HAL_CACHE_REGION_CONVERT         /* Enable mutual conversion between cacheable and non-cacheable addresses*/
#endif

#ifdef HAL_AUDIO_MODULE_ENABLED
#define HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT   /*Enable support multiple audio stream out feature.*/
#define HAL_AUDIO_SUPPORT_DEBUG_DUMP            /*Enable support dump audio register for debug.*/
#define HAL_AUDIO_SUPPORT_APLL                  /*Enable support apll feature.*/
//#define HAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE   /*Enable support multiple microphone.*/
//#define HAL_AUDIO_LEAKAGE_COMPENSATION_FEATURE   /*Enable support leakage compensation.*/
#endif

#ifdef HAL_ADC_MODULE_ENABLED
#define HAL_ADC_CALIBRATION_ENABLE               /*Enable ADC calibration */
#define HAL_ADC_SUPPORT_AVERAGE_ENABLE             /*Enable ADC support average    */
#endif

#ifdef HAL_I2C_MASTER_MODULE_ENABLED
#define HAL_I2C_MASTER_FEATURE_HIGH_SPEED       /* Enable I2C high speed 2M&3.25M. */
#define HAL_I2C_MASTER_FEATURE_SEND_TO_RECEIVE  /* Enable I2C master send to receive feature. */
#define HAL_I2C_MASTER_FEATURE_EXTENDED_DMA     /* Enable I2C master extend DMA feature.*/
#define HAL_I2C_MASTER_FEATURE_CONFIG_IO        /* Enable I2C master config IO mode feature.*/
#ifdef HAL_I2C_MASTER_FEATURE_EXTENDED_DMA
#define HAL_I2C_MASTER_FRATURE_NONE_BLOCKING    /* Enable I2C master software fifo */
#endif
#endif


#ifdef HAL_WDT_MODULE_ENABLED
#define HAL_WDT_FEATURE_SECOND_CHANNEL          /* Supports the second WDT */
#endif

#ifdef HAL_SPI_MASTER_MODULE_ENABLED
#define HAL_SPI_MASTER_FEATURE_ADVANCED_CONFIG       /* Enable SPI master advanced configuration feature. For more details, please refer to hal_spi_master.h.*/
#define HAL_SPI_MASTER_FEATURE_DEASSERT_CONFIG       /* Enable SPI master deassert configuration feature to deassert the chip select signal after each byte data transfer is complete. For more details, please refer to hal_spi_master.h.*/
#define HAL_SPI_MASTER_FEATURE_CHIP_SELECT_TIMING    /* Enable SPI master chip select timing configuration feature to set timing value for chip select signal. For more details, please refer to hal_spi_master.h.*/
#define HAL_SPI_MASTER_FEATURE_DMA_MODE              /* Enable SPI master DMA mode feature to do data transfer. For more details, please refer to hal_spi_master.h.*/
#define HAL_SPI_MASTER_FEATURE_DUAL_QUAD_MODE        /* Enable SPI master to use dual mode and quad mode. For more details, please refer to hal_spi_master.h. */
#define HAL_SPI_MASTER_FRATURE_NO_BUSY               /* Enable SPI master no busy API to support  multithreaded access. For more details, please refer to hal_spi_master.h. */
#endif

#ifdef HAL_ISINK_MODULE_ENABLED
#if defined(AB1565)
    #define HAL_ISINK_FEATURE_HW_PMIC2562
#endif
#define HAL_ISINK_FEATURE_ADVANCED_CONFIG           /*Enable ISINK advance config feature*/
#endif


#ifdef HAL_GPIO_MODULE_ENABLED
#define HAL_GPIO_FEATURE_PUPD               /* Pull state of the pin can be configured with different resistors through different combinations of GPIO_PUPD_x,GPIO_RESEN0_x and GPIO_RESEN1_x. For more details, please refer to hal_gpio.h. */
#define HAL_GPIO_FEATURE_CLOCKOUT           /* The pin can be configured as an output clock. For more details, please refer to hal_gpio.h. */
#define HAL_GPIO_FEATURE_HIGH_Z             /* The pin can be configured to provide high impedance state to prevent possible electric leakage. For more details, please refer to hal_gpio.h. */
#define HAL_GPIO_FEATURE_SET_DRIVING        /* The pin can be configured to enhance driving. For more details, please refer to hal_gpio.h. */
#define HAL_GPIO_FEATURE_SET_SCHMITT        /* The pin can be configured to enhance schmitt trigger hysteresis. */
#define HAL_GPIO_FEATURE_SET_SLEW_RATE      /* The pin can be configured to enhance slew rate. */
#endif

#ifdef HAL_EINT_MODULE_ENABLED
#define HAL_EINT_FEATURE_MASK                /* Supports EINT mask interrupt. */
#define HAL_EINT_FEATURE_SW_TRIGGER_EINT     /* Supports software triggered EINT interrupt. */
// #define HAL_EINT_FEATURE_MUX_MAPPING         /* Supports EINT number mux to different EINT GPIO pin. */
#endif

#ifdef HAL_ESC_MODULE_ENABLED
#define HAL_ESC_SUPPORT_FLASH                  /* Supports ESC with Flash. */
#define HAL_ESC_SUPPORT_PSRAM                  /* Supports ESC with PSRAM. */
#endif

#ifdef HAL_GPT_MODULE_ENABLED
#define HAL_GPT_FEATURE_US_TIMER               /* Supports a microsecond timer. */
#define HAL_GPT_SW_GPT_FEATURE                 /* Supports software GPT timer. */
#define HAL_GPT_PORT_ALLOCATE                  /* Allocates GPT communication port. */
#define HAL_GPT_SW_GPT_US_FEATURE              /* Supports software GPT us timer. */
#define HAL_GPT_SW_FEATURE_ABSOLUTE_COUNT       /* Supports software GPT absolute count in software GPT timeline */
#endif

#ifdef HAL_PWM_MODULE_ENABLED
#define HAL_PWM_FEATURE_ADVANCED_CONFIG        /* Supports PWM advanced configuration. */
#define HAL_PWM_FEATURE_FREQUENCY_DUTY         /* Supports PWM set frequency and duty. */
#endif

#ifdef HAL_RTC_MODULE_ENABLED
#define HAL_RTC_FEATURE_TIME_CALLBACK           /* Supports time change notification callback. */
#define HAL_RTC_FEATURE_RTC_MODE                /* Supports enter RTC mode. */
#define HAL_RTC_FEATURE_GPIO                    /* Supports RTC GPIO configuration. */
#define HAL_RTC_FEATURE_GPIO_EINT               /* Supports RTC GPIO and EINT configuration. */
#define HAL_RTC_FEATURE_CAPTOUCH                /* Supports CAPTOUCH configuration. */
#define HAL_RTC_FEATURE_EINT                    /* Supports EINT configuration. */
#define HAL_RTC_FEATURE_ALARM_BY_SECOND         /* Supports set rtc alarm by second. */
#define HAL_RTC_FEATURE_POWER_REASON            /* Supports get power on reason. */
#ifdef HAL_CAPTOUCH_MODULE_ENABLED
#define HAL_CPT_FEATURE_4CH                     /* Supports CAPTOUCH 4channel configuration. */
//#define HAL_CPT_FEATURE_8CH                     /* Supports CAPTOUCH 8channel configuration. */
#endif

#endif

#ifdef HAL_PWM_MODULE_ENABLED
#define HAL_PWM_FEATURE_ADVANCED_CONFIG        /* Supports PWM advanced configuration. */
#define HAL_PWM_FEATURE_FREQUENCY_DUTY         /* Supports PWM set frequency and duty. */
#define HAL_PWM_CLOCK_32K_SUPPORTED            /* Supports 32K PWM source clock. */
#define HAL_PWM_CLOCK_26M_SUPPORTED            /* Supports 26M PWM source clock. */
#define HAL_PWM_CLOCK_41M_SUPPORTED            /* Supports 41M PWM source clock. */
#endif


#ifdef HAL_SPI_SLAVE_MODULE_ENABLED
#define HAL_SPI_SLAVE_FEATURE_SW_CONTROL        /* Supports SPI slave to communicate with SPI master using software control. */
#define HAL_SPI_SLAVE_FEATURE_DIRECT_MODE       /* Supports SPI slave to communicate with SPI master without using software control. */
#define HAL_SPI_SLAVE_FEATURE_BYPASS            /* Supports SPI slave bypass feature. */
#endif


#ifdef HAL_UART_MODULE_ENABLED
#define HAL_UART_FEATURE_VFIFO_DMA_TIMEOUT        /* Supports configurable timeout value setting */
#define HAL_UART_FEATURE_3M_BAUDRATE              /* Supports UART 3M baudrate setting */
#define HAL_UART_FEATURE_6M_BAUDRATE               /* Supports UART 6M baudrate setting */
#define HAL_UART_FEATURE_FLOWCONTROL_CALLBACK      /* Supports enable UART flow control interrupt setting */
#endif

#ifdef HAL_AES_MODULE_ENABLED
#define HAL_AES_USE_PHYSICAL_MEMORY_ADDRESS        /* Notify caller must use physical memory */
#define HAL_AES_FEATURE_HWKEY_ENABLED              /* Supports HWKEY in AES module */
#define HAL_AES_FEATURE_CKDFKEY_ENABLED              /* Supports HWKEY in AES module */
#endif

#ifdef HAL_SHA_MODULE_ENABLED
#define HAL_SHA_USE_PHYSICAL_MEMORY_ADDRESS        /* Notify caller must use physical memory */
#endif

#ifdef HAL_SDIO_MODULE_ENABLED
#define HAL_SDIO_FEATURE_DATA1_IRQ                                  /*enable SDIO DAT1 IRQ feature*/
#endif

#ifdef HAL_SLEEP_MANAGER_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SLEEP_MANAGER
 * @{
 * @addtogroup hal_sleep_manager_enum Enum
 * @{
 */
/*****************************************************************************
 * Enum
 *****************************************************************************/
/** @brief Sleep modes */
typedef enum {
    HAL_SLEEP_MODE_NONE = 0,        /**< No sleep. */
    HAL_SLEEP_MODE_IDLE,            /**< Idle state. */
    HAL_SLEEP_MODE_SLEEP,           /**< Sleep state. */
    HAL_SLEEP_MODE_NUMBER           /**< To support range detection. */
} hal_sleep_mode_t;
/** @brief sleep_manager wake up source */
typedef enum {
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_GPT = 0,            /**< General purpose timer. */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_EINT = 1,           /**< External interrupt. */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_IRQGEN = 2,         /**< IRQGEN. */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_OST = 3,            /**< OST. */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_AUDIO = 4,          /**< AUDIO.  */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_I2S = 5,            /**< I2S. */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_I2S_DMA = 6,        /**< I2S_DMA. */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_CAP_TOUCH = 7,      /**< CAP_TOUCH. */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_ANC = 8,            /**< ANC. */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_SPI_SLAVE = 9,      /**< SPI_SLAVE. */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_RGU = 10,           /**< RGU. */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_DSP_DMA = 12,       /**< DSP_DMA. */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_ALL = 13            /**< All wakeup source. */
} hal_sleep_manager_wakeup_source_t;
/**
 * @}
 * @}
 * @}
 */
#endif

#ifdef HAL_UART_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup UART
 * @{
 * @addtogroup hal_uart_enum Enum
 * @{
 */
/*****************************************************************************
* UART
*****************************************************************************/
/** @brief UART port index
 * There are total of four UART ports. Only UART0 and UART1 support hardware flow control.
 * | UART port | Hardware Flow Control |
 * |-----------|-----------------------|
 * |  UART0    |           V           |
 * |  UART1    |           V           |
 * |  UART2    |           X           |
 * |  UART3    |           X           |
 */
typedef enum {
    HAL_UART_0 = 0,                            /**< UART port 0. */
    HAL_UART_1 = 1,                            /**< UART port 1. */
    HAL_UART_2 = 2,                            /**< UART port 2. */
    HAL_UART_MAX                               /**< The total number of UART ports (invalid UART port number). */
} hal_uart_port_t;

/**
  * @}
  */

/**@addtogroup hal_uart_define Define
 * @{
  */

/** @brief  The maximum timeout value for UART timeout interrupt, unit is ms.
  */
#define HAL_UART_TIMEOUT_VALUE_MAX  (2500)

/**
  * @}
  */

/**
 * @}
 * @}
 */
#endif


#ifdef HAL_I2C_MASTER_MODULE_ENABLED
#ifdef HAL_I2C_MASTER_FEATURE_EXTENDED_DMA
/**
 * @addtogroup HAL
 * @{
 * @addtogroup I2C_MASTER
 * @{
 * @section HAL_I2C_Transaction_Pattern_Chapter API transaction length and packets
 *  The Transaction packet is a packet sent by the I2C master using SCL and SDA.
 *  Different APIs have different transaction packets, as shown below.
 * - \b Transaction \b length \b supported \b by \b the \b APIs \n
 *  The total transaction length is determined by 4 parameters:\n
 *  send_packet_length(Ns), which indicates the number of sent packet.\n
 *  send_bytes_in_one_packet(Ms).\n
 *  receive_packet_length(Nr).\n
 *  receive_bytes_in_one_packet(Mr).\n
 *  Next, the relationship between transaction packet and these 4 parameters is introduced.
 *  - Total transaction length = Ns * Ms + Nr * Mr.
 *   - Ns is the packet length to be sent by the I2C master.
 *   - Ms is the total number of bytes in a sent packet.
 *   - Nr is the packet length to be received by the I2C master.
 *   - Mr is the total number of bytes in a received packet.
 *  - NA means the related parameter should be ignored.
 *  - 1~8 specifies the parameter range is from 1 to 8. 1~15 specifies the parameter range is from 1 to 15. 1~255 specifies the parameter range from 1 to 255.
 *  - 1 means the parameter value can only be 1.
 *  - Note1: functions with the suffix "_ex" have these 4 parameters. Other functions only have the "size" parameter and the driver splits the "size" into these 4 parameters.
 *  - Note2: The maximum of total transaction length is 256K.\n
 *    #hal_i2c_master_send_polling() for example, the "size" will be divided like this: send_packet_length = 1, send_bytes_in_one_packet = size.
 *          As a result, the total size should be: send_packet_length * send_bytes_in_one_packet = 1 * size = size. The range of "size" should be from 1 to 8.
 * |API                                         |send_packet_length(Ns) | send_bytes_in_one_packet(Ms) | receive_packet_length(Nr) | receive_bytes_in_one_packet(Mr) |
 * |--------------------------------------------|-----------------------|------------------------------|---------------------------|---------------------------------|
 * |hal_i2c_master_send_polling                 |          1            |            1~8               |            NA             |                NA               |
 * |hal_i2c_master_receive_polling              |          NA           |            NA                |            1              |                1~8              |
 * |hal_i2c_master_send_to_receive_polling      |          1            |            1~8               |            1              |                1~8              |
 * |hal_i2c_master_send_dma                     |          1            |            1~65535           |            NA             |                NA               |
 * |hal_i2c_master_receive_dma                  |          NA           |            NA                |            1~65535        |                1                |
 * |hal_i2c_master_send_to_receive_dma          |          1            |            1~65535           |            1~65534        |                1                |
 * |hal_i2c_master_send_dma_ex                  |          1~65535      |            1~65535           |            NA             |                NA               |
 * |hal_i2c_master_receive_dma_ex               |          NA           |            NA                |            1~65535        |                1~65535          |
 * |hal_i2c_master_send_to_receive_dma_ex       |          1            |            1~65535           |            1~65534        |                1~65535          |
 *
 * - \b Waveform \b pattern \b supported \b by \b the \b APIs \n
 *  The 4 parameters (send_packet_length(Ns), send_bytes_in_one_packet(Ms), receive_packet_length(Nr), receive_bytes_in_one_packet(Mr) will also affect the transaction packet.
 *  The relationship between transaction packet and these 4 parameters is shown below.
 *  - Ns is the send_packet_length.
 *  - Ms is the send_bytes_in_one_packet.
 *  - Nr is the receive_packet_length.
 *  - Mr is the receive_bytes_in_one_packet.
 * |API                                          |transaction packet format                                 |
 * |---------------------------------------------|----------------------------------------------------------|
 * | hal_i2c_master_send_polling                 |  @image html hal_i2c_send_poling_waveform.png            |
 * | hal_i2c_master_receive_polling              |  @image html hal_i2c_receive_poling_waveform.png         |
 * | hal_i2c_master_send_to_receive_polling      |  @image html hal_i2c_send_to_receive_poling_waveform.png |
 * | hal_i2c_master_send_dma                     |  @image html hal_i2c_send_dma_waveform.png            |
 * | hal_i2c_master_receive_dma                  |  @image html hal_i2c_receive_dma_waveform.png         |
 * | hal_i2c_master_send_to_receive_dma          |  @image html hal_i2c_send_to_receive_dma_waveform.png |
 * | hal_i2c_master_send_dma_ex                  |  @image html hal_i2c_send_dma_ex_waveform.png            |
 * | hal_i2c_master_receive_dma_ex               |  @image html hal_i2c_receive_dma_ex_waveform.png         |
 * | hal_i2c_master_send_to_receive_dma_ex       |  @image html hal_i2c_send_to_receive_dma_ex_waveform.png |
 *
 *
 *
 *
 */
#endif

/** @defgroup hal_i2c_master_define Define
 * @{
  */

/** @brief  The maximum polling mode transaction size.
  */
#define HAL_I2C_MAXIMUM_POLLING_TRANSACTION_SIZE  8

/** @brief  The maximum DMA mode transaction size.
  */
#define HAL_I2C_MAXIMUM_DMA_TRANSACTION_SIZE   65535

/**
  * @}
  */

/** @addtogroup hal_i2c_master_enum Enum
  * @{
  */

/*****************************************************************************
* I2C master
*****************************************************************************/
/** @brief This enum defines the I2C port.
 *  The platform supports 4 I2C masters. Three of them support polling mode and DMA mode,
 *  while the other only supports polling mode. For more information about the polling mode,
 *  DMA mode, queue mode, please refer to @ref HAL_I2C_Features_Chapter. The details
 *  are shown below:
 *  - Supported features of I2C masters \n
 *    V : supported;  X : not supported.
 * |I2C Master   | Polling mode | DMA mode | Extended DMA mode |
 * |-------------|--------------|----------|-------------------|
 * |I2C0         |      V       |    V     |         V         |
 * |I2C1         |      V       |    V     |         V         |
 * |I2C2         |      V       |    V     |         V         |
 * |I2CAO        |      X       |    X     |         X         |
 *
 *
*/
typedef enum {
    HAL_I2C_MASTER_0 = 0,                /**< I2C master 0. */
    HAL_I2C_MASTER_1 = 1,                /**< I2C master 1. */
    HAL_I2C_MASTER_2 = 2,                /**< I2C master 2. */
    HAL_I2C_MASTER_AO = 3,               /**< I2C master AO. */
    HAL_I2C_MASTER_MAX                   /**< The total number of I2C masters (invalid I2C master number). */
} hal_i2c_port_t;

/**
  * @}
  */

/**
 * @}
 * @}
 */
#endif


#ifdef HAL_GPIO_MODULE_ENABLED
/**
* @addtogroup HAL
* @{
* @addtogroup GPIO
* @{
*
* @addtogroup hal_gpio_enum Enum
* @{
*/

/*****************************************************************************
* GPIO
*****************************************************************************/
/** @brief This enum defines the GPIO port.
 *  The platform supports a total of 49 GPIO pins with various functionality.
 *
*/

typedef enum {
    HAL_GPIO_0  = 0,    /**< GPIO pin0. */
    HAL_GPIO_1  = 1,    /**< GPIO pin1. */
    HAL_GPIO_2  = 2,    /**< GPIO pin2. */
    HAL_GPIO_3  = 3,    /**< GPIO pin3. */
    HAL_GPIO_4  = 4,    /**< GPIO pin4. */
    HAL_GPIO_5  = 5,    /**< GPIO pin5. */
    HAL_GPIO_6  = 6,    /**< GPIO pin6. */
    HAL_GPIO_7  = 7,    /**< GPIO pin7. */
    HAL_GPIO_8  = 8,    /**< GPIO pin8. */
    HAL_GPIO_9  = 9,    /**< GPIO pin9. */
    HAL_GPIO_10 = 10,   /**< GPIO pin10. */
    HAL_GPIO_11 = 11,   /**< GPIO pin11. */
    HAL_GPIO_12 = 12,   /**< GPIO pin12. */
    HAL_GPIO_13 = 13,   /**< GPIO pin13. */
    HAL_GPIO_14 = 14,   /**< GPIO pin14. */
    HAL_GPIO_15 = 15,   /**< GPIO pin15. */
    HAL_GPIO_16 = 16,   /**< GPIO pin16. */
    HAL_GPIO_17 = 17,   /**< GPIO pin17. */
    HAL_GPIO_18 = 18,   /**< GPIO pin18. */
    HAL_GPIO_19 = 19,   /**< GPIO pin19. */
#if (defined(MT2822) || defined(AB1568))
    HAL_GPIO_20 = 20,   /**< GPIO pin20. */
    HAL_GPIO_21 = 21,   /**< GPIO pin21. */
    HAL_GPIO_22 = 22,   /**< GPIO pin22. */
    HAL_GPIO_23 = 23,   /**< GPIO pin23. */
    HAL_GPIO_24 = 24,   /**< GPIO pin24. */
    HAL_GPIO_25 = 25,   /**< GPIO pin25. */
    HAL_GPIO_26 = 26,   /**< GPIO pin26. */
#endif
#ifdef MT2822A
    HAL_GPIO_27 = 27,   /**< GPIO pin27. */
    HAL_GPIO_28 = 28,   /**< GPIO pin28. */
    HAL_GPIO_29 = 29,   /**< GPIO pin29. */
    HAL_GPIO_30 = 30,   /**< GPIO pin30. */
    HAL_GPIO_31 = 31,   /**< GPIO pin31. */
    HAL_GPIO_32 = 32,   /**< GPIO pin32. */
    HAL_GPIO_33 = 33,   /**< GPIO pin33. */
    HAL_GPIO_34 = 34,   /**< GPIO pin34. */
    HAL_GPIO_35 = 35,   /**< GPIO pin35. */
    HAL_GPIO_36 = 36,   /**< GPIO pin36. */
    HAL_GPIO_37 = 37,   /**< GPIO pin37. */
    HAL_GPIO_38 = 38,   /**< GPIO pin38. */
    HAL_GPIO_39 = 39,   /**< GPIO pin39. */
#endif
    HAL_GPIO_MAX                               /**< The total number of GPIO pins (invalid GPIO pin number). */
} hal_gpio_pin_t;

/**
  * @}
  */

/**
 * @}
 * @}
 */
#endif

#ifdef HAL_GPIO_FEATURE_CLOCKOUT
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPIO
 * @{
 * @addtogroup hal_gpio_enum Enum
 * @{
 */
/*****************************************************************************
* CLKOUT
*****************************************************************************/
/** @brief  This enum defines output clock number of GPIO */
typedef enum {
    HAL_GPIO_CLOCK_0   = 0,              /**< define GPIO output clock 0 */
    HAL_GPIO_CLOCK_1   = 1,              /**< define GPIO output clock 1 */
    HAL_GPIO_CLOCK_2   = 2,              /**< define GPIO output clock 2 */
    HAL_GPIO_CLOCK_3   = 3,              /**< define GPIO output clock 3 */
    HAL_GPIO_CLOCK_MAX                   /**< define GPIO output clock max number(invalid) */
} hal_gpio_clock_t;


/** @brief This enum defines output clock mode of GPIO */
typedef enum {
    HAL_GPIO_CLOCK_MODE_32K = 1,        /**< define GPIO output clock mode as 32KHz */
    HAL_GPIO_CLOCK_MODE_26M = 2,        /**< define GPIO output clock mode as 26MHz */
    HAL_GPIO_CLOCK_MODE_13M = 3,        /**< define GPIO output clock mode as 13MHz */
    HAL_GPIO_CLOCK_MODE_41M = 4,        /**< define GPIO output clock mode as 41.6MHz */
    HAL_GPIO_CLOCK_MODE_10M = 6,        /**< define GPIO output clock mode as 10.4MHz */
    HAL_GPIO_CLOCK_MODE_MAX             /**< define GPIO output clock mode of max number(invalid) */
} hal_gpio_clock_mode_t;
/**
  * @}
  */

/**
 * @}
 * @}
 */
#endif

#ifdef HAL_GPIO_FEATURE_SET_DRIVING
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPIO
 * @{
 * @addtogroup hal_gpio_enum Enum
 * @{
 */
/** @brief This enum defines driving current. */
typedef enum {
    HAL_GPIO_DRIVING_CURRENT_2MA    = 0,        /**< Defines GPIO driving current as 2mA.  */
    HAL_GPIO_DRIVING_CURRENT_4MA    = 1,        /**< Defines GPIO driving current as 4mA.  */
    HAL_GPIO_DRIVING_CURRENT_6MA    = 2,        /**< Defines GPIO driving current as 6mA. */
    HAL_GPIO_DRIVING_CURRENT_8MA    = 3         /**< Defines GPIO driving current as 8mA. */
} hal_gpio_driving_current_t;
/**
  * @}
  */

/**
 * @}
 * @}
 */
#endif

#ifdef HAL_ADC_MODULE_ENABLED

/**
* @addtogroup HAL
* @{
* @addtogroup ADC
* @{
*
* @addtogroup hal_adc_enum Enum
* @{
*/

/*****************************************************************************
* ADC
*****************************************************************************/
/** @brief adc channel */
typedef enum {
    HAL_ADC_CHANNEL_0 = 0,                        /**< ADC channel 0. */
    HAL_ADC_CHANNEL_1 = 1,                        /**< ADC channel 1. */
    HAL_ADC_CHANNEL_2 = 2,                        /**< ADC channel 2. */
    HAL_ADC_CHANNEL_3 = 3,                        /**< ADC channel 3. */
    HAL_ADC_CHANNEL_4 = 4,                        /**< ADC channel 4. */
    HAL_ADC_CHANNEL_5 = 5,                        /**< ADC channel 5. */
    HAL_ADC_CHANNEL_6 = 6,                        /**< ADC channel 6. */
    HAL_ADC_CHANNEL_7 = 7,                        /**< ADC channel 7. */
    HAL_ADC_CHANNEL_MAX                           /**< The total number of ADC channels (invalid ADC channel).*/
} hal_adc_channel_t;

/**
  * @}
  */


/**
 * @}
 * @}
 */
#endif



#ifdef HAL_I2S_MODULE_ENABLED
/**
* @addtogroup HAL
* @{
* @addtogroup I2S
* @{
*
* @addtogroup hal_i2s_enum Enum
* @{
*/


/*****************************************************************************
* I2S
*****************************************************************************/
#ifdef HAL_I2S_FEATURE_MULTI_I2S
/** @brief This enum defines the I2S port.
 *
 *  The platform supports 2 I2S HW interface. Two of them support master mode and slave mode.
 *  User can drive 2 I2S HW simultaneously by the extended APIs. The basic APIs are only for I2S0.
 *  User should not use basic APIs and extension APIs simultaneously. The details are shown below:
 *
 *  - I2S supported feature table \n
 *    V : means support;  X : means not support.
 *
 * |I2S PORT   |SAMPLE WIDTH    | FS | I2S TDM  |
 * |--------- |--------------|-------------|-------------------|
 * |I2S0         | 16 bits                 |11.025 / 16 / 22.05 / 24 / 44.1 /48 /96 / 192 KHZ     | V        |
 * |I2S1         | 16 / 24 bits         |11.025 / 16 / 22.05 / 24 / 44.1 /48 /96 / 192 KHZ     | X        |
 *
*/
typedef enum {
    HAL_I2S_0  = 0,   /**< I2S interfeace 0. */
    HAL_I2S_1  = 1,    /**< I2S interfeace 1. */
    HAL_I2S_MAX
} hal_i2s_port_t;
#endif


#ifdef HAL_I2S_FEATURE_TDM
/** @brief Channels per frame sync. Number of channels in each frame sync.*/
typedef enum {
    HAL_I2S_TDM_2_CHANNEL  = 0,   /**< 2 channels. */
    HAL_I2S_TDM_4_CHANNEL  = 1    /**< 4 channels. */
} hal_i2s_tdm_channel_t;


/** @brief Polarity of BCLK.*/
typedef enum {
    HAL_I2S_BCLK_INVERSE_DISABLE  = 0, /**< Normal mode. (Invalid)*/
    HAL_I2S_BCLK_INVERSE_EABLE  = 1    /**< Invert BCLK. (Invalid)*/
} hal_i2s_bclk_inverse_t;
#endif

#ifdef HAL_I2S_EXTENDED
/** @brief I2S sample widths.  */
typedef enum {
    HAL_I2S_SAMPLE_WIDTH_8BIT  = 0,   /**< I2S sample width is 8bit. (Invalid)*/
    HAL_I2S_SAMPLE_WIDTH_16BIT = 1,   /**< I2S sample width is 16bit. (HAL_I2S_0)*/
    HAL_I2S_SAMPLE_WIDTH_24BIT = 2    /**< I2S sample width is 24bit. (HAL_I2S_0/HAL_I2S_1)*/
} hal_i2s_sample_width_t;


/** @brief Number of bits per frame sync(FS). This parameter determines the bits of a complete sample of both left and right channels.*/
typedef enum {
    HAL_I2S_FRAME_SYNC_WIDTH_32  = 0,   /**< 32 bits per frame. */
    HAL_I2S_FRAME_SYNC_WIDTH_64  = 1,   /**< 64 bits per frame. */
    HAL_I2S_FRAME_SYNC_WIDTH_128  = 2   /**< 128 bits per frame. */
} hal_i2s_frame_sync_width_t;


/** @brief Enable or disable right channel of the I2S TX to send the same data as on the left channel of the I2S TX.\n
        This function only works when the sample width of the I2S is 16 bits.*/
typedef enum {
    HAL_I2S_TX_MONO_DUPLICATE_DISABLE = 0,  /**< Keep data to its channel. */
    HAL_I2S_TX_MONO_DUPLICATE_ENABLE  = 1   /**< Assume input is mono data for left channel, the data is duplicated to right channel.*/
} hal_i2s_tx_mode_t;


/** @brief Enable or disable twice the downsampling rate mode in the I2S RX.
                 In this mode the sampling rate of the I2S TX is 48kHz while the sampling rate of the I2S RX is 24kHz. The I2S RX automatically drops 1 sample in each 2 input samples received. */
typedef enum {
    HAL_I2S_RX_DOWN_RATE_DISABLE = 0,  /**< Actual sampling rate of the I2S RX = sampling rate. (Default)*/
    HAL_I2S_RX_DOWN_RATE_ENABLE  = 1   /**< Actual sampling rate of the I2S RX is half of the original sampling rate. (Invalid)*/
} hal_i2s_rx_down_rate_t;
#endif //  #ifdef HAL_I2S_EXTENDED


/** @brief Enable or disable data swapping between right and left channels of the I2S link.\n
        This function only works when the sample width of the I2S is 16 bits.*/
typedef enum {
    HAL_I2S_LR_SWAP_DISABLE = 0,  /**< Disable the data swapping. */
    HAL_I2S_LR_SWAP_ENABLE  = 1   /**< Enable the data swapping.  */
} hal_i2s_lr_swap_t;


/** @brief Enable or disable word select clock inverting of the I2S link. */
typedef enum {
    HAL_I2S_WORD_SELECT_INVERSE_DISABLE = 0,  /**< Disable the word select clock inverting. */
    HAL_I2S_WORD_SELECT_INVERSE_ENABLE  = 1   /**< Enable the word select clock inverting.  */
} hal_i2s_word_select_inverse_t;

/** @brief This enum defines initial type of the I2S.
 */

typedef enum {
    HAL_I2S_TYPE_EXTERNAL_MODE          = 0,        /**< External mode. I2S mode with external codec.*/
    HAL_I2S_TYPE_EXTERNAL_TDM_MODE      = 1,        /**< External TDM mode. I2S TDM mode with external codec*/
    HAL_I2S_TYPE_INTERNAL_MODE          = 2,        /**< Internal mode. I2S mode with internal codec. (Invalid)*/
    HAL_I2S_TYPE_INTERNAL_LOOPBACK_MODE = 3,        /**< I2S internal loopback mode. Data loopback mode.*/
    HAL_I2S_TYPE_INTERNAL_TDM_LOOPBACK_MODE = 4,    /**< TDM internal loopback mode. Data loopback mode.*/
    HAL_I2S_TYPE_MAX = 5
} hal_i2s_initial_type_t;


/** @brief I2S event */
typedef enum {
    HAL_I2S_EVENT_ERROR               = -1, /**<  An error occurred during the function call. */
    HAL_I2S_EVENT_NONE                =  0, /**<  No error occurred during the function call. */
    HAL_I2S_EVENT_OVERFLOW            =  1, /**<  RX data overflow. */
    HAL_I2S_EVENT_UNDERFLOW           =  2, /**<  TX data underflow. */
    HAL_I2S_EVENT_DATA_REQUEST        =  3, /**<  Request for user-defined data. */
    HAL_I2S_EVENT_DATA_NOTIFICATION   =  4  /**<  Notify user the RX data is ready. */
} hal_i2s_event_t;


/** @brief I2S sampling rates */
typedef enum {
    HAL_I2S_SAMPLE_RATE_8K        = 0,  /**<  8000Hz  */
    HAL_I2S_SAMPLE_RATE_11_025K   = 1,  /**<  11025Hz */
    HAL_I2S_SAMPLE_RATE_12K       = 2,  /**<  12000Hz */
    HAL_I2S_SAMPLE_RATE_16K       = 3,  /**<  16000Hz */
    HAL_I2S_SAMPLE_RATE_22_05K    = 4,  /**<  22050Hz */
    HAL_I2S_SAMPLE_RATE_24K       = 5,  /**<  24000Hz */
    HAL_I2S_SAMPLE_RATE_32K       = 6,  /**<  32000Hz */
    HAL_I2S_SAMPLE_RATE_44_1K     = 7,  /**<  44100Hz */
    HAL_I2S_SAMPLE_RATE_48K       = 8,  /**<  48000Hz */
    HAL_I2S_SAMPLE_RATE_88_2K     = 9,  /**<  88200Hz */
    HAL_I2S_SAMPLE_RATE_96K       = 10, /**<  96000Hz */
    HAL_I2S_SAMPLE_RATE_176_4K    = 11, /**<  176400Hz */
    HAL_I2S_SAMPLE_RATE_192K      = 12  /**<  192000Hz */
} hal_i2s_sample_rate_t;

/**
  * @}
  */

/**
 * @}
 * @}
 */
#endif


#ifdef HAL_SPI_MASTER_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SPI_MASTER
 * @{
 * @defgroup hal_spi_master_define Define
 * @{
 */

/** @brief  The maximum polling mode transaction size in bytes.
 */
#define HAL_SPI_MAXIMUM_POLLING_TRANSACTION_SIZE  32

/** @brief  The maximum transaction size in bytes when configuration is not single mode.
 */
#define HAL_SPI_MAXIMUM_NON_SINGLE_MODE_TRANSACTION_SIZE  15

/** @brief  The minimum clock frequency.
 */
#define  HAL_SPI_MASTER_CLOCK_MIN_FREQUENCY  30000

/** @brief  The maximum clock frequency.
 */
#define  HAL_SPI_MASTER_CLOCK_MAX_FREQUENCY  52000000

/**
 * @}
 */

/**
 * @addtogroup hal_spi_master_enum Enum
 * @{
 */

/*****************************************************************************
* SPI master
*****************************************************************************/
/** @brief This enum defines the SPI master port.
 *  The chip supports total of 4 SPI master ports, each of them supports polling mode
 *  and DMA mode. For more details about polling mode and DMA mode, please refer to @ref
 *  HAL_SPI_MASTER_Features_Chapter.
 */
typedef enum {
    HAL_SPI_MASTER_0 = 0,                              /**< SPI master port 0. */
    HAL_SPI_MASTER_1 = 1,                              /**< SPI master port 1. */
    HAL_SPI_MASTER_2 = 2,                              /**< SPI master port 2. */
    HAL_SPI_MASTER_MAX                                 /**< The total number of SPI master ports (invalid SPI master port). */
} hal_spi_master_port_t;

/** @brief This enum defines the options to connect the SPI slave device to the SPI master's CS pins. */
typedef enum {
    HAL_SPI_MASTER_SLAVE_0 = 0,                       /**< The SPI slave device is connected to the SPI master's CS0 pin. */
    HAL_SPI_MASTER_SLAVE_1 = 1,                       /**< The SPI slave device is connected to the SPI master's CS1 pin. */
    HAL_SPI_MASTER_SLAVE_2 = 2,                       /**< The SPI slave device is connected to the SPI master's CS2 pin. */
    HAL_SPI_MASTER_SLAVE_3 = 3,                       /**< The SPI slave device is connected to the SPI master's CS3 pin. */
    HAL_SPI_MASTER_SLAVE_MAX                          /**< The total number of SPI master CS pins (invalid SPI master CS pin). */
} hal_spi_master_slave_port_t;

/** @brief SPI master transaction bit order definition. */
typedef enum {
    HAL_SPI_MASTER_LSB_FIRST = 0,                       /**< Both send and receive data transfer LSB first. */
    HAL_SPI_MASTER_MSB_FIRST = 1                        /**< Both send and receive data transfer MSB first. */
} hal_spi_master_bit_order_t;

/** @brief SPI master clock polarity definition. */
typedef enum {
    HAL_SPI_MASTER_CLOCK_POLARITY0 = 0,                     /**< Clock polarity is 0. */
    HAL_SPI_MASTER_CLOCK_POLARITY1 = 1                      /**< Clock polarity is 1. */
} hal_spi_master_clock_polarity_t;

/** @brief SPI master clock format definition. */
typedef enum {
    HAL_SPI_MASTER_CLOCK_PHASE0 = 0,                         /**< Clock format is 0. */
    HAL_SPI_MASTER_CLOCK_PHASE1 = 1                          /**< Clock format is 1. */
} hal_spi_master_clock_phase_t;

/** @brief This enum defines the mode of the SPI master. */
typedef enum {
    HAL_SPI_MASTER_SINGLE_MODE = 0,                      /**< Single mode. */
    HAL_SPI_MASTER_3_WIRE_MODE = 1,                      /**< Normal mode. */
    HAL_SPI_MASTER_DUAL_MODE = 2,                        /**< Dual mode. */
    HAL_SPI_MASTER_QUAD_MODE = 3,                        /**< Quad mode. */
} hal_spi_master_mode_t;

/**
 * @}
 */

/**
 * @}
 * @}
 */
#endif

#ifdef HAL_SPI_SLAVE_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SPI_SLAVE
 * @{
 * @addtogroup hal_spi_slave_enum Enum
 * @{
 */

/*****************************************************************************
* SPI slave
*****************************************************************************/
/** @brief This enum defines the SPI slave port. This chip supports only one
 *  SPI slave port.
 */
typedef enum {
    HAL_SPI_SLAVE_0 = 0,                             /**< SPI slave port 0. */
    HAL_SPI_SLAVE_MAX                                /**< The total number of SPI slave ports (invalid SPI slave port number). */
} hal_spi_slave_port_t;

/** @brief SPI slave transaction bit order definition. */
typedef enum {
    HAL_SPI_SLAVE_LSB_FIRST = 0,                       /**< Both send and receive data transfer is the LSB first. */
    HAL_SPI_SLAVE_MSB_FIRST = 1                        /**< Both send and receive data transfer is the MSB first. */
} hal_spi_slave_bit_order_t;

/** @brief SPI slave clock polarity definition. */
typedef enum {
    HAL_SPI_SLAVE_CLOCK_POLARITY0 = 0,                 /**< Clock polarity is 0. */
    HAL_SPI_SLAVE_CLOCK_POLARITY1 = 1                  /**< Clock polarity is 1. */
} hal_spi_slave_clock_polarity_t;

/** @brief SPI slave clock format definition. */
typedef enum {
    HAL_SPI_SLAVE_CLOCK_PHASE0 = 0,                    /**< Clock format is 0. */
    HAL_SPI_SLAVE_CLOCK_PHASE1 = 1                     /**< Clock format is 1. */
} hal_spi_slave_clock_phase_t;

/** @brief This enum defines the SPI slave event when an interrupt occurs. */
typedef enum {
    HAL_SPI_SLAVE_EVENT_POWER_ON = SPIS_INT_POWER_ON_MASK,         /**< Power on command is received. */
    HAL_SPI_SLAVE_EVENT_POWER_OFF = SPIS_INT_POWER_OFF_MASK,       /**< Power off command is received. */
    HAL_SPI_SLAVE_EVENT_CRD_FINISH = SPIS_INT_RD_CFG_FINISH_MASK,  /**< Configure read command is received. */
    HAL_SPI_SLAVE_EVENT_RD_FINISH = SPIS_INT_RD_TRANS_FINISH_MASK, /**< Read command is received. */
    HAL_SPI_SLAVE_EVENT_CWR_FINISH = SPIS_INT_WR_CFG_FINISH_MASK,  /**< Configure write command is received. */
    HAL_SPI_SLAVE_EVENT_WR_FINISH = SPIS_INT_WR_TRANS_FINISH_MASK, /**< Write command is received. */
    HAL_SPI_SLAVE_EVENT_RD_ERR = SPIS_INT_RD_DATA_ERR_MASK,        /**< An error occurred during a read command. */
    HAL_SPI_SLAVE_EVENT_WR_ERR = SPIS_INT_WR_DATA_ERR_MASK,        /**< An error occurred during a write command. */
    HAL_SPI_SLAVE_EVENT_TIMEOUT_ERR = SPIS_INT_TMOUT_ERR_MASK,     /**< A timeout is detected between configure read command and read command or configure write command and write command. */
    HAL_SPI_SLAVE_EVENT_IDLE_TIMEOUT = SPIS_INT_IDLE_TMOUT_MASK,   /**< A timeout is detected as CS pin is inactive in direct mode. */
    HAL_SPI_SLAVE_EVENT_TX_DMA_EMPTY = SPIS_INT_TX_DMA_EMPTY_MASK,      /**< The data in VFIFO TX buffer is less than tx threshold in DMA direct mode. */
    HAL_SPI_SLAVE_EVENT_RX_DMA_FULL = SPIS_INT_RX_DMA_FULL_MASK,        /**< The data in RX fifo is larger than rx threshold in DMA direct mode. */
    HAL_SPI_SLAVE_EVENT_RX_OVERRUN = SPIS_INT_RX_OVERRUN_MASK,          /**< A data received when the data in VFIFO RX buffer is equal to the avilable fifo space in direct mode. */
} hal_spi_slave_callback_event_t;

/** @brief This enum defines the SPI slave commands. */
typedef enum {
    HAL_SPI_SLAVE_CMD_WS        = 0,       /**< Write Status command. */
    HAL_SPI_SLAVE_CMD_RS        = 1,       /**< Read Status command. */
    HAL_SPI_SLAVE_CMD_WR        = 2,       /**< Write Data command. */
    HAL_SPI_SLAVE_CMD_RD        = 3,       /**< Read Data command. */
    HAL_SPI_SLAVE_CMD_POWEROFF  = 4,       /**< POWER OFF command. */
    HAL_SPI_SLAVE_CMD_POWERON   = 5,       /**< POWER ON command. */
    HAL_SPI_SLAVE_CMD_CW        = 6,       /**< Configure Write command. */
    HAL_SPI_SLAVE_CMD_CR        = 7,        /**< Configure Read command. */
    HAL_SPI_SLAVE_CMD_CT        = 8        /**< Configure Type command. */
} hal_spi_slave_command_type_t;

#ifdef HAL_SPI_SLAVE_FEATURE_BYPASS
/** @brief This enum defines the SPI slave bypass master port. */
typedef enum {
    HAL_SPI_SLAVE_BYPASS_MASTER_0 = 0,      /**< SPI slave bypass master port 0. */
    HAL_SPI_SLAVE_BYPASS_MASTER_1 = 1,      /**< SPI slave bypass master port 1. */
    HAL_SPI_SLAVE_BYPASS_MASTER_2 = 2,      /**< SPI slave bypass master port 2. */
    HAL_SPI_SLAVE_BYPASS_MASTER_MAX         /**< The total number of SPI slave bypass destination ports (invalid SPI slave bypass destination port number). */
} hal_spi_slave_bypass_master_port_t;

/** @brief This enum defines the SPI slave bypass master port chip selection channels.
 */
typedef enum
{
  HAL_SPI_SLAVE_BYPASS_MASTER_CS_0 = 0,     /**< bypass SPI slave chip selection channel to master chip selection channel 0. */
  HAL_SPI_SLAVE_BYPASS_MASTER_CS_1 = 1,     /**< bypass SPI slave chip selection channel to master chip selection channel 1. */
  HAL_SPI_SLAVE_BYPASS_MASTER_CS_2 = 2,     /**< bypass SPI slave chip selection channel to master chip selection channel 2. */
  HAL_SPI_SLAVE_BYPASS_MASTER_CS_3 = 3,     /**< bypass SPI slave chip selection channel to master chip selection channel 3. */
  HAL_SPI_SLAVE_BYPASS_MASTER_CS_MAX,       /**< The total number of SPI slave bypass chip selection channel */
}hal_spi_slave_bypass_master_cs_t;

/** @brief This enum defines the SPI slave bypass mode. */
typedef enum
{
  HAL_SPI_SLAVE_BYPASS_MODE_W  = 0,       /**< The signal MOSI/MISO/SIO2/SIO3 of SPI slave bypass to SPI Master MOSI/MISO/SIO2/SIO3. @image html hal_spi_slave_bypass_mode_w.png*/
  HAL_SPI_SLAVE_BYPASS_MODE_R  = 1,       /**< The signal MOSI/MISO/SIO2/SIO3 of SPI master bypass to SPI slave MOSI/MISO/SIO2/SIO3. @image html hal_spi_slave_bypass_mode_r.png*/
  HAL_SPI_SLAVE_BYPASS_MODE_RW = 2,       /**< The signal bypass bi-directionally. The signal MOSI of SPI slave bypass to SPI master, and the signal MISO of SPI master bypass to SPI slave. @image html hal_spi_slave_bypass_mode_rw.png */
  HAL_SPI_SLAVE_BYPASS_MODE_MAX,          /**< The total number of SPI slave bypass mode. */
}hal_spi_slave_bypass_mode_t;
#endif

/**
 * @}
 */


/**
 * @}
 * @}
 */
#endif


#ifdef HAL_RTC_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup RTC
 * @{
 * @addtogroup hal_rtc_define Define
 * @{
 */

/** @brief  This macro defines a maximum number for backup data that used in #hal_rtc_set_data(),
  * #hal_rtc_get_data(), #hal_rtc_clear_data functions.
  */
#define HAL_RTC_BACKUP_BYTE_NUM_MAX     (12)

/**
 * @}
 */

/**
 * @defgroup hal_rtc_enum Enum
 * @{
 */

/** @brief RTC current time change notification period selections. */
typedef enum {
    HAL_RTC_TIME_NOTIFICATION_NONE = 0,                     /**< No need for a time notification. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_SECOND = 1,             /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every second. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_MINUTE = 2,             /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every minute. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_HOUR = 3,               /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every hour. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_DAY = 4,                /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every day. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_MONTH = 5,              /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every month. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_YEAR = 6,               /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every year. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_SECOND_1_2 = 7,         /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every one-half of a second. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_SECOND_1_4 = 8,         /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every one-fourth of a second. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_SECOND_1_8 = 9,         /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every one-eighth of a second. */
    HAL_RTC_TIME_NOTIFICATION_MAX,                          /**< Max enum item  */
} hal_rtc_time_notification_period_t;
/** @brief This enum defines the type of RTC GPIO. */
typedef enum {
    HAL_RTC_GPIO_0 = 0,     /**< RTC GPIO 0. */
    HAL_RTC_GPIO_1 = 1,     /**< RTC GPIO 1. */
    HAL_RTC_GPIO_2 = 2,     /**< RTC GPIO 2. */
    HAL_RTC_GPIO_MAX
} hal_rtc_gpio_t;
/** @brief This enum defines the data type of RTC GPIO. */
typedef enum {
    HAL_RTC_GPIO_DATA_LOW  = 0,                     /**< RTC GPIO data low. */
    HAL_RTC_GPIO_DATA_HIGH = 1                      /**< RTC GPIO data high. */
} hal_rtc_gpio_data_t;
/**
 * @}
 */

/** @defgroup hal_rtc_struct Struct
  * @{
  */

/** @brief RTC time structure definition. */
typedef struct {
    uint8_t rtc_sec;                                  /**< Seconds after minutes     - [0,59]  */
    uint8_t rtc_min;                                  /**< Minutes after the hour    - [0,59]  */
    uint8_t rtc_hour;                                 /**< Hours after midnight      - [0,23]  */
    uint8_t rtc_day;                                  /**< Day of the month          - [1,31]  */
    uint8_t rtc_mon;                                  /**< Months                    - [1,12]  */
    uint8_t rtc_week;                                 /**< Days in a week            - [0,6]   */
    uint8_t rtc_year;                                 /**< Years                     - [0,127] */
    uint16_t rtc_milli_sec;                           /**< Millisecond value, when in time API, this represents the read only register rtc_int_cnt - [0,32767]; when in alarm API, this parameter has no meaning. */
} hal_rtc_time_t;
/** @brief RTC GPIO control structure definition. */
typedef struct {
    hal_rtc_gpio_t rtc_gpio;        /**< Configure which GPIO will apply this setting. */
    bool is_enable_rtc_eint;        /**< Enable RTC EINT or not. */
    bool is_falling_edge_active;    /**< Configure RTC EINT as falling edge active or not. */
    bool is_enable_debounce;        /**< Enable RTC EINT debounce or not, if enabled, EINT debounce time is 4T*32k. */
} hal_rtc_eint_config_t;
/** @brief This structure defines the settings of RTC GPIO. */
typedef struct {
    hal_rtc_gpio_t rtc_gpio;        /**< Configure which GPIO will apply this setting. */
    bool is_analog;                 /**< Cinfugure RTC GPIO as analog or digtal mode. */
    bool is_input;                  /**< Cinfugure RTC GPIO as input direction or not. */
    bool is_pull_up;                /**< If RTC GPIO is pull mode, configure RTC GPIO pull up or not. */
    bool is_pull_down;              /**< If RTC GPIO is pull mode, configure RTC GPIO pull down or not. */
} hal_rtc_gpio_config_t;
/**
 * @}
 */

/**
 * @}
 * @}
 */
#endif


#ifdef HAL_EINT_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup EINT
 * @{
 * @addtogroup hal_eint_enum Enum
 * @{
 */

/*****************************************************************************
* EINT
*****************************************************************************/
/** @brief EINT pins. */
typedef enum {
    HAL_EINT_NUMBER_0 = 0,
    HAL_EINT_NUMBER_1 = 1,
    HAL_EINT_NUMBER_2 = 2,
    HAL_EINT_NUMBER_3 = 3,
    HAL_EINT_NUMBER_4 = 4,
    HAL_EINT_NUMBER_5 = 5,
    HAL_EINT_NUMBER_6 = 6,
    HAL_EINT_NUMBER_7 = 7,
    HAL_EINT_NUMBER_8 = 8,
    HAL_EINT_NUMBER_9 = 9,
    HAL_EINT_NUMBER_10 = 10,
    HAL_EINT_NUMBER_11 = 11,
    HAL_EINT_NUMBER_12 = 12,
    HAL_EINT_NUMBER_13 = 13,
    HAL_EINT_NUMBER_14 = 14,
    HAL_EINT_NUMBER_15 = 15,
    HAL_EINT_NUMBER_16 = 16,
    HAL_EINT_NUMBER_17 = 17,
    HAL_EINT_NUMBER_18 = 18,
    HAL_EINT_NUMBER_19 = 19,
#if (defined(MT2822) || defined(AB1568))
    HAL_EINT_NUMBER_20 = 20,
    HAL_EINT_NUMBER_21 = 21,
    HAL_EINT_NUMBER_22 = 22,
    HAL_EINT_NUMBER_23 = 23,
    HAL_EINT_NUMBER_24 = 24,
    HAL_EINT_NUMBER_25 = 25,
    HAL_EINT_NUMBER_26 = 26,
#endif
#ifdef MT2822A
    HAL_EINT_NUMBER_27 = 27,
    HAL_EINT_NUMBER_28 = 28,
    HAL_EINT_NUMBER_29 = 29,
    HAL_EINT_NUMBER_30 = 30,
    HAL_EINT_NUMBER_31 = 31,
    HAL_EINT_NUMBER_32 = 32,
    HAL_EINT_NUMBER_33 = 33,
    HAL_EINT_NUMBER_34 = 34,
    HAL_EINT_NUMBER_35 = 35,
    HAL_EINT_NUMBER_36 = 36,
    HAL_EINT_NUMBER_37 = 37,
    HAL_EINT_NUMBER_38 = 38,
    HAL_EINT_NUMBER_39 = 39,
#endif
    HAL_EINT_UART_0_RX = 40,    /**< EINT number 40:  UART0 RX. */
    HAL_EINT_UART_1_RX = 41,    /**< EINT number 41:  UART1 RX. */
    HAL_EINT_UART_2_RX = 42,    /**< EINT number 42:  UART2 RX. */
    HAL_EINT_RTC       = 43,    /**< EINT number 43:  RTC. */
    HAL_EINT_USB       = 44,    /**< EINT number 44:  USB. */
    HAL_EINT_PMU       = 45,    /**< EINT number 45:  PMIC. */
    HAL_EINT_NUMBER_MAX         /**< The total number of EINT channels (invalid EINT channel). */
} hal_eint_number_t;
/**
  * @}
  */
/**
 * @}
 * @}
 */
#endif

#ifdef HAL_GPT_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 * @addtogroup hal_gpt_enum Enum
 * @{
 */

/*****************************************************************************
* GPT
*****************************************************************************/
/** @brief GPT port */
typedef enum {
    HAL_GPT_0 = 0,                          /**< GPT port 0. CM4 User defined. */
    HAL_GPT_1 = 1,                          /**< GPT port 1. Used for CM4 as software GPT. The clock source is 1Mhz.*/
    HAL_GPT_2 = 2,                          /**< GPT port 2. Usee for CM4/DSP0 to set a microsecond delay and get microsecond free count. The clock source is 1Mhz*/
    HAL_GPT_3 = 3,                          /**< GPT port 3. Used for CM4 as software GPT. The clock source is 32Khz.*/
    HAL_GPT_4 = 4,                          /**< GPT port 4. Usee for CM4/DSP0 to set a millisecond delay and get 1/32Khz free count. The clock source is 32Khz*/
    HAL_GPT_5 = 5,                          /**< GPT port 5. Used for DSP0 as software GPT. The clock source is 32Khz.*/
    HAL_GPT_6 = 6,                          /**< GPT port 6. Used for DSP0 as software GPT. The clock source is 1Mhz.*/
    HAL_GPT_7 = 7,                          /**< GPT port 7. CM4 User defined.*/
    HAL_GPT_8 = 8,                          /**< GPT port 8. Used for BT system wakeup*/
    HAL_GPT_MAX_PORT = 9,                  /**< The total number of GPT ports (invalid GPT port). */
    HAL_GPT_MAX = 9
} hal_gpt_port_t;

/** @brief GPT clock source  */
typedef enum {
    HAL_GPT_CLOCK_SOURCE_32K = 0,            /**< Set the GPT clock source to 32kHz, 1 tick = 1/32768 seconds. */
    HAL_GPT_CLOCK_SOURCE_1M  = 1             /**< Set the GPT clock source to 1MHz, 1 tick = 1 microsecond.*/
} hal_gpt_clock_source_t;

/** @brief  The maximum time of millisecond timer.
  */
#define HAL_GPT_MAXIMUM_MS_TIMER_TIME   (130150523)

/** @brief  The max user number for sw gpt,include ms user and us user.
  */
#define HAL_SW_GPT_MAX_USERS 20

/**
  * @}
  */
/**
 * @}
 * @}
 */
#endif

#ifdef HAL_FLASH_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup FLASH
 * @{
 */

/*****************************************************************************
* Flash
*****************************************************************************/

/** @defgroup hal_flash_define Define
 * @{
  */

/** @brief  This macro defines the Flash base address.
  */
#define HAL_FLASH_BASE_ADDRESS    (0x08000000)
/**
  * @}
  */
/**
 * @}
 * @}
 */
#endif


#ifdef HAL_ESC_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup ESC
 * @{
 */

/** @defgroup hal_esc_define Define
 * @{
  */

/** @brief  This macro defines the ESC memory base address.
  */
#define HAL_ESC_BASE_ADDRESS    (0x05000000)
/**
  * @}
  */

/**
 * @}
 * @}
 */
#endif


#ifdef HAL_PWM_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup PWM
 * @{
 * @addtogroup hal_pwm_enum Enum
 * @{
 */
/*****************************************************************************
* PWM
*****************************************************************************/
/** @brief The PWM channels */
typedef enum {
    HAL_PWM_0 = 0,                            /**< PWM channel 0. */
    HAL_PWM_1 = 1,                            /**< PWM channel 1. */
    HAL_PWM_2 = 2,                            /**< PWM channel 2. */
    HAL_PWM_3 = 3,                            /**< PWM channel 3. */
    HAL_PWM_4 = 4,                            /**< PWM channel 4. */
    HAL_PWM_MAX_CHANNEL                     /**< The total number of PWM channels (invalid PWM channel).*/
} hal_pwm_channel_t;

/** @brief PWM clock source options */
typedef enum {
    HAL_PWM_CLOCK_26MHZ = 0,                /**< PWM clock source 26MHz(FXO). */
    HAL_PWM_CLOCK_26MHZ_OSC = 1,            /**< PWM clock source 26MHz(OSC). */
    HAL_PWM_CLOCK_32KHZ = 2,                /**< PWM clock srouce 32kHz. */
    HAL_PWM_CLOCK_41MHZ = 3,                /**< PWM clock srouce 41.6MHz. */
    HAL_PWM_CLOCK_MAX,
} hal_pwm_source_clock_t ;

/**
  * @}
  */
/**
 * @}
 * @}
 */
#endif

#ifdef HAL_WDT_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup WDT
 * @{
*/

/* @addtogroup hal_wdt_define Define
 * @{
 */
/** @brief  This enum define the max timeout value of WDT.  */
#define HAL_WDT_MAX_TIMEOUT_VALUE (1000)
/**
 * @}
 */

/**
 * @}
 * @}
 */

#endif

#ifdef HAL_CACHE_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup CACHE
 * @{
 */

/*****************************************************************************
* Cache
*****************************************************************************/
/* NULL */

/**
 * @}
 * @}
 */
#endif

#define audio_message_type_t hal_audio_message_type_t
#ifdef HAL_AUDIO_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup AUDIO
 * @{
 * @addtogroup hal_audio_enum Enum
 * @{
 */
/** @brief AUDIO port */
typedef enum {
    HAL_AUDIO_STREAM_OUT1    = 0, /**<  stream out HWGAIN1 only. */
    HAL_AUDIO_STREAM_OUT2    = 1, /**<  stream out HWGAIN2 only. */
    HAL_AUDIO_STREAM_OUT_ALL = 2, /**<  stream out HWGAIN1 & HWGAIN2 & HWGAIN3. */
    HAL_AUDIO_STREAM_OUT3    = 3, /**<  stream out HWGAIN3 only. */
} hal_audio_hw_stream_out_index_t;

/** @brief Audio message type */
typedef enum {
    AUDIO_MESSAGE_TYPE_COMMON,            /**< Audio basic scenario. */
    AUDIO_MESSAGE_TYPE_BT_AUDIO_UL  = 1,  /**< BT audio UL scenario. */
    AUDIO_MESSAGE_TYPE_BT_AUDIO_DL  = 2,  /**< BT audio DL scenario. */
    AUDIO_MESSAGE_TYPE_BT_VOICE_UL  = 3,  /**< BT aoice UL scenario. */
    AUDIO_MESSAGE_TYPE_BT_VOICE_DL  = 4,  /**< BT aoice DL scenario. */
    AUDIO_MESSAGE_TYPE_PLAYBACK     = 5,  /**< Local playback scenario. */
    AUDIO_MESSAGE_TYPE_RECORD       = 6,  /**< Mic record scenario. */
    AUDIO_MESSAGE_TYPE_PROMPT       = 7,  /**< Voice prompt scenario. */
    AUDIO_MESSAGE_TYPE_LINEIN       = 8,  /**< LineIN & loopback scenario. */
    AUDIO_MESSAGE_TYPE_BLE_AUDIO_UL = 9,  /**< BLE audio UL scenario. */
    AUDIO_MESSAGE_TYPE_BLE_AUDIO_DL = 10, /**< BLE audio DL scenario. */
    AUDIO_MESSAGE_TYPE_SIDETONE,          /**< Sidetone scenario. */
    AUDIO_MESSAGE_TYPE_ANC,               /**< ANC scenario. */
    AUDIO_MESSAGE_TYPE_AFE,               /**< DSP AFE dummy type. */
    AUDIO_MESSAGE_TYPE_AUDIO_TRANSMITTER= 14, /**< audio transmitter scenario.**/
#ifdef AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
    AUDIO_MESSAGE_TYPE_PROMPT_DUMMY_SOURCE, /**< audio transmitter send scenario.**/
#endif
    AUDIO_MESSAGE_TYPE_SPDIF_DUMP,        /**< DSP AFE dummy type. */
    AUDIO_MESSAGE_TYPE_ADAPTIVE,          /**< Adaptive control */
    AUDIO_MESSAGE_TYPE_MCLK,              /**< DSP AFE dummy type. */
    AUDIO_MESSAGE_TYPE_AUDIO_NVDM,        /**< audio nvdm relative. */
    AUDIO_MESSAGE_TYPE_BT_A2DP_DL,        /**< audio a2dp sync action scenario.**/
    AUDIO_MESSAGE_TYPE_MAX,               /**< Audio scenario type MAX. */

    AUDIO_RESERVE_TYPE_QUERY_RCDC,        /**< Query Message: RCDC. Different from above audio main scenario messages. Only for query purpose.*/
    AUDIO_RESERVE_TYPE_ULL_QUERY_RCDC,    /**< Query Message: ULL RCDC. Different from above audio main scenario messages. Only for query purpose.*/
    AUDIO_MESSAGE_TYPE_BLE_AUDIO_SUB_UL,  /**< BLE audio SUB UL scenario. */
    AUDIO_MESSAGE_TYPE_BLE_AUDIO_SUB_DL,  /**< BLE audio SUB DL scenario. */

    AUDIO_MESSAGE_TYPE_ANC_MONITOR_ADAPTIVE_ANC, /**< Adaptive ANC scenario. */

} audio_message_type_t;

/*****************************************************************************
* Audio setting
*****************************************************************************/

#ifdef HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT
/** @brief Audio device. */
typedef enum {
    HAL_AUDIO_DEVICE_NONE               = 0x0000,  /**<  No audio device is on. */
    HAL_AUDIO_DEVICE_MAIN_MIC_L         = 0x0001,  /**<  Stream in: main mic L. */
    HAL_AUDIO_DEVICE_MAIN_MIC_R         = 0x0002,  /**<  Stream in: main mic R. */
    HAL_AUDIO_DEVICE_MAIN_MIC_DUAL      = 0x0003,  /**<  Stream in: main mic L+R. */
    HAL_AUDIO_DEVICE_LINEINPLAYBACK_L   = 0x0004,  /**<  Stream in: line in playback L. */
    HAL_AUDIO_DEVICE_LINEINPLAYBACK_R   = 0x0008,  /**<  Stream in: line in playback R. */
    HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL= 0x000c,  /**<  Stream in: line in playback L+R. */
    HAL_AUDIO_DEVICE_USBAUDIOPLAYBACK_L   = 0x0014,  /**<  Stream in: usb audio playback L. */
    HAL_AUDIO_DEVICE_USBAUDIOPLAYBACK_R   = 0x0018,  /**<  Stream in: usb audio playback R. */
    HAL_AUDIO_DEVICE_USBAUDIOPLAYBACK_DUAL= 0x001c,  /**<  Stream in: usb audio playback L+R. */
    HAL_AUDIO_DEVICE_DIGITAL_MIC_L      = 0x0010,  /**<  Stream in: digital mic L. */
    HAL_AUDIO_DEVICE_DIGITAL_MIC_R      = 0x0020,  /**<  Stream in: digital mic R. */
    HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL   = 0x0030,  /**<  Stream in: digital mic L+R. */

    HAL_AUDIO_DEVICE_DAC_L              = 0x0100,  /**<  Stream out:speaker L. */
    HAL_AUDIO_DEVICE_DAC_R              = 0x0200,  /**<  Stream out:speaker R. */
    HAL_AUDIO_DEVICE_DAC_DUAL           = 0x0300,  /**<  Stream out:speaker L+R. */

    HAL_AUDIO_DEVICE_I2S_MASTER         = 0x1000,  /**<  Stream in/out: I2S master role */
    HAL_AUDIO_DEVICE_I2S_SLAVE          = 0x2000,  /**<  Stream in/out: I2S slave role */
    HAL_AUDIO_DEVICE_EXT_CODEC          = 0x3000,   /**<  Stream out: external amp.&codec, stereo/mono */

    HAL_AUDIO_DEVICE_I2S_MASTER_L       = 0x10000, /**<  Stream in/out: I2S master L */
    HAL_AUDIO_DEVICE_I2S_MASTER_R       = 0x20000, /**<  Stream in/out: I2S master R */

    HAL_AUDIO_DEVICE_MAIN_MIC           = 0x0001,       /**<  OLD: Stream in: main mic. */
    HAL_AUDIO_DEVICE_HEADSET_MIC        = 0x0002,       /**<  OLD: Stream in: earphone mic. */
    HAL_AUDIO_DEVICE_HANDSET            = 0x0004,       /**<  OLD: Stream out:receiver. */
    HAL_AUDIO_DEVICE_HANDS_FREE_MONO    = 0x0008,       /**<  OLD: Stream out:loudspeaker, mono. */
    HAL_AUDIO_DEVICE_HANDS_FREE_STEREO  = 0x0010,       /**<  OLD: Stream out:loudspeaker, stereo to mono L=R=(R+L)/2. */
    HAL_AUDIO_DEVICE_HEADSET            = 0x0020,       /**<  OLD: Stream out:earphone, stereo */
    HAL_AUDIO_DEVICE_HEADSET_MONO       = 0x0040,       /**<  OLD: Stream out:earphone, mono to stereo. L=R. */
    HAL_AUDIO_DEVICE_LINE_IN            = 0x0080,       /**<  OLD: Stream in/out: line in. */
    HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC   = 0x0100,       /**<  OLD: Stream in: dual digital mic. */
    HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC = 0x0200,       /**<  OLD: Stream in: single digital mic. */

    HAL_AUDIO_DEVICE_DUMMY              = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} hal_audio_device_t;
#endif
/** @brief Hal audio analog mode setting. */
typedef enum {
    HAL_AUDIO_ANALOG_INPUT_ACC10K   = 0,            /**<   for amic mode*/
    HAL_AUDIO_ANALOG_INPUT_ACC20K   = 1,            /**<   for amic mode*/
    HAL_AUDIO_ANALOG_INPUT_DCC      = 2,            /**<   for amic mode*/

    HAL_AUDIO_ANALOG_OUTPUT_CLASSG  = 0,            /**<   for dac mode*/
    HAL_AUDIO_ANALOG_OUTPUT_CLASSAB = 1,            /**<   for dac mode*/
    HAL_AUDIO_ANALOG_OUTPUT_CLASSD  = 2,            /**<   for dac mode*/
    HAL_AUDIO_ANALOG_MODE_DUMMY     = 0x7FFFFFFF,   /**<  Dummy for DSP structure alignment */
} hal_audio_analog_mdoe_t;
/** @brief Hal audio dmic selection. */
typedef enum {
    HAL_AUDIO_DMIC_GPIO_DMIC0   = 0x0,              /**<  for dmic selection */
    HAL_AUDIO_DMIC_GPIO_DMIC1,                      /**<  for dmic selection */
    HAL_AUDIO_DMIC_ANA_DMIC0,                       /**<  for dmic selection */
    HAL_AUDIO_DMIC_ANA_DMIC1,                       /**<  for dmic selection */
    HAL_AUDIO_DMIC_ANA_DMIC2,                       /**<  for dmic selection */
    HAL_AUDIO_DMIC_ANA_DMIC3,                       /**<  for dmic selection */
    HAL_AUDIO_DMIC_ANA_DMIC4,                       /**<  for dmic selection */
    HAL_AUDIO_DMIC_ANA_DMIC5,                       /**<  for dmic selection */
    HAL_AUDIO_DMIC_DUMMY        = 0xFFFFFFFF,           /**<  for DSP structrue alignment */
} hal_audio_dmic_selection_t;

/** @brief Hal audio performance mode setting. */
typedef enum {
    AFE_PEROFRMANCE_NORMAL_MODE     = 0,
    AFE_PEROFRMANCE_HIGH_MODE       = 1,
    //The followings are only for UL
    AFE_PEROFRMANCE_LOW_POWER_MODE  = 2,
    AFE_PEROFRMANCE_ULTRA_LOW_POWER_MODE  = 3,
    AFE_PEROFRMANCE_SUPER_ULTRA_LOW_POWER_MODE  = 4,
    AFE_PEROFRMANCE__DUMMY     = 0x7FFFFFFF,   /**<  Dummy for DSP structure alignment */
} hal_audio_performance_mode_t;

/** @brief audio channel selection define */
typedef enum {
    HAL_AUDIO_DIRECT                     = 0, /**< A single interconnection, output equal to input. */
    HAL_AUDIO_SWAP_L_R                   = 2, /**< L and R channels are swapped. That is (L, R) -> (R, L). */
    HAL_AUDIO_BOTH_L                     = 3, /**< only output L channel. That is (L, R) -> (L, L). */
    HAL_AUDIO_BOTH_R                     = 4, /**< only output R channel. That is (L, R) -> (R, R). */
    HAL_AUDIO_MIX_L_R                    = 5, /**< L and R channels are mixed. That is (L, R) -> (L+R, L+R). */
    HAL_AUDIO_MIX_SHIFT_L_R              = 6, /**< L and R channels are mixed and shift. That is (L, R) -> (L/2+R/2, L/2+R/2). */
    HAL_AUDIO_CHANNEL_SELECTION_DUMMY    = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} hal_audio_channel_selection_t;

/** @brief i2s clk source define */
typedef enum {
    I2S_CLK_SOURCE_APLL                         = 0, /**< Low jitter mode. */
    I2S_CLK_SOURCE_DCXO                         = 1, /**< Normal mode. */
    I2S_CLK_SOURCE_TYPE_DUMMY                   = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} I2S_CLK_SOURCE_TYPE;

/** @brief micbias source define */
typedef enum {
    MICBIAS_SOURCE_0                            = 1, /**< Open micbias0. */
    MICBIAS_SOURCE_1                            = 2, /**< Open micbias1. */
    MICBIAS_SOURCE_2                            = 3, /**< Open micbias2. */
    MICBIAS_SOURCE_3                            = 4, /**< Open micbias3. */
    MICBIAS_SOURCE_4                            = 5, /**< Open micbias4. */
    MICBIAS_SOURCE_ALL                          = 6, /**< Open micbias0 to micbias4. */
    MICBIAS_SOURCE_TYPE_DUMMY                   = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} MICBIAS_SOURCE_TYPE;

/** @brief micbias out voltage define */
typedef enum {
    MICBIAS3V_OUTVOLTAGE_1p8v                   = 1 << 2,   /**< 1.8V */
    MICBIAS3V_OUTVOLTAGE_1p85v                  = 1 << 3,   /**< 1.85V (Default) */
    MICBIAS3V_OUTVOLTAGE_1p9v                   = 1 << 4,   /**< 1.9V */
    MICBIAS3V_OUTVOLTAGE_2p0v                   = 1 << 5,   /**< 2.0V */
    MICBIAS3V_OUTVOLTAGE_2p1v                   = 1 << 6,   /**< 2.1V */
    MICBIAS3V_OUTVOLTAGE_2p2v                   = 1 << 7,   /**< 2.2V (Not support in 2V) */
    MICBIAS3V_OUTVOLTAGE_2p4v                   = 1 << 8,   /**< 2.4V (Not support in 2V) */
    MICBIAS3V_OUTVOLTAGE_2p55v                  = 1 << 9,   /**< 2.55V (Not support in 2V) */
    MICBIAS3V_OUTVOLTAGE_VCC                    = 0x7f<< 2, /**< BYPASSEN  */
    MICBIAS3V_OUTVOLTAGE_TYPE_DUMMY             = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} MICBIAS3V_OUTVOLTAGE_TYPE;

/** @brief micbias0 amic type define */
typedef enum {
    MICBIAS0_AMIC_MEMS                          = 0 << 10,  /**< MEMS (Default)*/
    MICBIAS0_AMIC_ECM_DIFFERENTIAL              = 1 << 10,  /**< ECM Differential*/
    MICBIAS0_AMIC_ECM_SINGLE                    = 3 << 10,  /**< ECM Single*/
    MICBIAS0_AMIC_TYPE_DUMMY                    = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} MICBIAS0_AMIC_TYPE;

/** @brief micbias1 amic type define */
typedef enum {
    MICBIAS1_AMIC_MEMS                          = 0 << 12,  /**< MEMS (Default)*/
    MICBIAS1_AMIC_ECM_DIFFERENTIAL              = 1 << 12,  /**< ECM Differential*/
    MICBIAS1_AMIC_ECM_SINGLE                    = 3 << 12,  /**< ECM Single*/
    MICBIAS1_AMIC_TYPE_DUMMY                    = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} MICBIAS1_AMIC_TYPE;

/** @brief uplink performance type define */
typedef enum {
    UPLINK_PERFORMANCE_NORMAL                   = 0 << 13, /**< Normal mode (Default)*/
    UPLINK_PERFORMANCE_HIGH                     = 1 << 13, /**< High performance mode*/
    UPLINK_PERFORMANCE_TYPE_DUMMY               = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} UPLINK_PERFORMANCE_TYPE;

/** @brief amic mic type define */
typedef enum {
    ANALOG_INPUT_MODE_DCC                                    = 0 << 14, /**< AMIC DCC mode.*/
    ANALOG_INPUT_MODE_ACC_10K                                = 1 << 14, /**< AMIC ACC 10K mode.*/
    ANALOG_INPUT_MODE_ACC_20K                                = 2 << 14, /**< AMIC ACC 10K mode.*/
    ANALOG_INPUT_MODE_DUMMY                                  = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} ANALOG_INPUT_MODE;

/** @brief dac output mode define */
typedef enum {
    ANALOG_OUTPUT_MODE_CLASSAB = 0 << 16,
    ANALOG_OUTPUT_MODE_CLASSG =  0 << 16,
    ANALOG_OUTPUT_MODE_CLASSD  = 1 << 16,
}ANALOG_OUTPUT_MODE;

/** @brief downlink performance type define */
typedef enum {
    DOWNLINK_PERFORMANCE_NORMAL                 = 0, /**< Normal mode (Default)*/
    DOWNLINK_PERFORMANCE_HIGH                   = 1, /**< High performance mode*/
    DOWNLINK_PERFORMANCE_TYPE_DUMMY             = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} DOWNLINK_PERFORMANCE_TYPE;

/** @brief audio MCLK pin select define */
typedef enum {
    AFE_MCLK_PIN_FROM_I2S0 = 0,     /**< MCLK from I2S0's mclk pin */
    AFE_MCLK_PIN_FROM_I2S1,         /**< MCLK from I2S1's mclk pin */
    AFE_MCLK_PIN_FROM_I2S2,         /**< MCLK from I2S2's mclk pin */
    AFE_MCLK_PIN_FROM_I2S3,         /**< MCLK from I2S3's mclk pin */
} afe_mclk_out_pin_t;

/** @brief audio APLL define */
typedef enum {
    AFE_APLL_NOUSE = 0,
    AFE_APLL1 = 1,                  /**< APLL1:45.1584M, 44.1K base */
    AFE_APLL2 = 2,                  /**< APLL2:49.152M, 48K base */
} afe_apll_source_t;

/** @brief audio MCLK status define */
typedef enum {
    MCLK_DISABLE = 0,               /**< Turn off MCLK */
    MCLK_ENABLE  = 1,               /**< Turn on MCLK */
} afe_mclk_status_t;

/** @brief amp performance define */
typedef enum {
    AUDIO_AMP_PERFORMANCE_NORMAL                = 0, /**< Normal mode. */
    AUDIO_AMP_PERFORMANCE_HIGH                  = 1, /**< High performance mode. */
    AUDIO_AMP_PERFORMANCE_TYPE_DUMMY            = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} AUDIO_AMP_PERFORMANCE_TYPE;

/** @brief audio ul1 dmic data selection define */
typedef enum {
    HAL_AUDIO_UL1_DMIC_DATA_SHIFT_NUM           = 15,
    HAL_AUDIO_UL1_DMIC_DATA_GPIO_DMIC0          = 0 << HAL_AUDIO_UL1_DMIC_DATA_SHIFT_NUM,    /**< Select UL1 data from GPIO DMIC0 */
    HAL_AUDIO_UL1_DMIC_DATA_GPIO_DMIC1          = 1 << HAL_AUDIO_UL1_DMIC_DATA_SHIFT_NUM,    /**< Select UL1 data from GPIO DMIC1 */
    HAL_AUDIO_UL1_DMIC_DATA_ANA_DMIC0           = 2 << HAL_AUDIO_UL1_DMIC_DATA_SHIFT_NUM,    /**< Select UL1 data from ANA_DMIC0 */
    HAL_AUDIO_UL1_DMIC_DATA_ANA_DMIC1           = 3 << HAL_AUDIO_UL1_DMIC_DATA_SHIFT_NUM,    /**< Select UL1 data from ANA_DMIC1 */
    HAL_AUDIO_UL1_DMIC_DATA_MASK                = HAL_AUDIO_UL1_DMIC_DATA_ANA_DMIC1,
} hal_audio_ul1_dmic_data_selection_t;

/** @brief audio ul2 dmic data selection define */
typedef enum {
    HAL_AUDIO_UL2_DMIC_DATA_SHIFT_NUM           = 17,
    HAL_AUDIO_UL2_DMIC_DATA_GPIO_DMIC0          = 0 << HAL_AUDIO_UL2_DMIC_DATA_SHIFT_NUM,    /**< Select UL2 data from GPIO DMIC0 */
    HAL_AUDIO_UL2_DMIC_DATA_GPIO_DMIC1          = 1 << HAL_AUDIO_UL2_DMIC_DATA_SHIFT_NUM,    /**< Select UL2 data from GPIO DMIC1 */
    HAL_AUDIO_UL2_DMIC_DATA_ANA_DMIC0           = 2 << HAL_AUDIO_UL2_DMIC_DATA_SHIFT_NUM,    /**< Select UL2 data from ANA_DMIC0 */
    HAL_AUDIO_UL2_DMIC_DATA_ANA_DMIC1           = 3 << HAL_AUDIO_UL2_DMIC_DATA_SHIFT_NUM,    /**< Select UL2 data from ANA_DMIC1 */
    HAL_AUDIO_UL2_DMIC_DATA_MASK                = HAL_AUDIO_UL2_DMIC_DATA_ANA_DMIC1,
} hal_audio_ul2_dmic_data_selection_t;

/** @brief audio ul3 dmic data selection define */
typedef enum {
    HAL_AUDIO_UL3_DMIC_DATA_SHIFT_NUM           = 19,
    HAL_AUDIO_UL3_DMIC_DATA_GPIO_DMIC0          = 0 << HAL_AUDIO_UL3_DMIC_DATA_SHIFT_NUM,    /**< Select UL3 data from GPIO DMIC0 */
    HAL_AUDIO_UL3_DMIC_DATA_GPIO_DMIC1          = 1 << HAL_AUDIO_UL3_DMIC_DATA_SHIFT_NUM,    /**< Select UL3 data from GPIO DMIC1 */
    HAL_AUDIO_UL3_DMIC_DATA_ANA_DMIC0           = 2 << HAL_AUDIO_UL3_DMIC_DATA_SHIFT_NUM,    /**< Select UL3 data from ANA_DMIC0 */
    HAL_AUDIO_UL3_DMIC_DATA_ANA_DMIC1           = 3 << HAL_AUDIO_UL3_DMIC_DATA_SHIFT_NUM,    /**< Select UL3 data from ANA_DMIC1 */
    HAL_AUDIO_UL3_DMIC_DATA_MASK                = HAL_AUDIO_UL3_DMIC_DATA_ANA_DMIC1,
} hal_audio_ul3_dmic_data_selection_t;

/** @brief DSP streaming source channel define */
typedef enum {
    AUDIO_DSP_CHANNEL_SELECTION_STEREO          = 0, /**< DSP streaming output L and R will be it own. */
    AUDIO_DSP_CHANNEL_SELECTION_MONO            = 1, /**< DSP streaming output L and R will be (L+R)/2. */
    AUDIO_DSP_CHANNEL_SELECTION_BOTH_L          = 2, /**< DSP streaming output both L. */
    AUDIO_DSP_CHANNEL_SELECTION_BOTH_R          = 3, /**< DSP streaming output both R. */
    AUDIO_DSP_CHANNEL_SELECTION_NUM,
} AUDIO_DSP_CHANNEL_SELECTION;

/** @brief Hal audio bias voltage. */
typedef enum {
    HAL_AUDIO_BIAS_VOLTAGE_1_80V    = 0x0,            /**<  for bias voltage setting */
    HAL_AUDIO_BIAS_VOLTAGE_1_85V,                     /**<  for bias voltage setting */
    HAL_AUDIO_BIAS_VOLTAGE_1_90V,                     /**<  for bias voltage setting */
    HAL_AUDIO_BIAS_VOLTAGE_2_00V,                     /**<  for bias voltage setting */
    HAL_AUDIO_BIAS_VOLTAGE_2_10V,                     /**<  for bias voltage setting */
    HAL_AUDIO_BIAS_VOLTAGE_2_20V,                     /**<  for bias voltage setting */
    HAL_AUDIO_BIAS_VOLTAGE_2_40V,                     /**<  for bias voltage setting */
    HAL_AUDIO_BIAS_VOLTAGE_2_55V,                     /**<  for bias voltage setting */
    HAL_AUDIO_BIAS_VOLTAGE_DUMMY    = 0xFFFFFFFF,       /**<  for DSP structrue alignment */
} hal_audio_bias_voltage_t;

/** @brief Hal audio bias selection. */
typedef enum {
    HAL_AUDIO_BIAS_SELECT_BIAS0 = 1<<0,                 /**< Open micbias0. */
    HAL_AUDIO_BIAS_SELECT_BIAS1 = 1<<1,                 /**< Open micbias1. */
    HAL_AUDIO_BIAS_SELECT_BIAS2 = 1<<2,                 /**< Open micbias2. */
    HAL_AUDIO_BIAS_SELECT_BIAS3 = 1<<3,                 /**< Open micbias3. */
    HAL_AUDIO_BIAS_SELECT_BIAS4 = 1<<4,                 /**< Open micbias4. */
    HAL_AUDIO_BIAS_SELECT_MAX   = HAL_AUDIO_BIAS_SELECT_BIAS4,
    HAL_AUDIO_BIAS_SELECT_ALL   = 0x1F,                  /**< Open micbias0 and micbias1 and micbias2. */
    HAL_AUDIO_BIAS_SELECT_NUM   = 5,
    HAL_AUDIO_BIAS_SELECT_DUMMY = 0xFFFFFFFF,           /**<  for DSP structrue alignment */
} hal_audio_bias_selection_t;

/** @brief Hal audio ul path iir filter. */
typedef enum {
    HAL_AUDIO_UL_IIR_DISABLE        = 0xF,            /**< UL IIR setting */
    HAL_AUDIO_UL_IIR_SW             = 0x0,            /**< UL IIR setting */
    HAL_AUDIO_UL_IIR_5HZ_AT_48KHZ   = 0x1,            /**< UL IIR setting */
    HAL_AUDIO_UL_IIR_10HZ_AT_48KHZ  = 0x2,            /**< UL IIR setting */
    HAL_AUDIO_UL_IIR_25HZ_AT_48KHZ  = 0x3,            /**< UL IIR setting */
    HAL_AUDIO_UL_IIR_50HZ_AT_48KHZ  = 0x4,            /**< UL IIR setting */
    HAL_AUDIO_UL_IIR_75HZ_AT_48KHZ  = 0x5,            /**< UL IIR setting */
    HAL_AUDIO_UL_IIR_100HZ_AT_48KHZ = 0x6,            /**< UL IIR setting */
    HAL_AUDIO_UL_IIR_125HZ_AT_48KHZ = 0x7,            /**< UL IIR setting */
    HAL_AUDIO_UL_IIR_150HZ_AT_48KHZ = 0x8,            /**< UL IIR setting */
    HAL_AUDIO_UL_IIR_175HZ_AT_48KHZ = 0x9,            /**< UL IIR setting */
    HAL_AUDIO_UL_IIR_200HZ_AT_48KHZ = 0xA,            /**< UL IIR setting */
    HAL_AUDIO_UL_IIR_225HZ_AT_48KHZ = 0xB,            /**< UL IIR setting */
    HAL_AUDIO_UL_IIR_250HZ_AT_48KHZ = 0xC,            /**< UL IIR setting */
    HAL_AUDIO_UL_IIR_275HZ_AT_48KHZ = 0xD,            /**< UL IIR setting */
    HAL_AUDIO_UL_IIR_300HZ_AT_48KHZ = 0xE,            /**< UL IIR setting */

    HAL_AUDIO_UL_IIR_10HZ_AT_96KHZ  = 0x1,            /**< UL IIR setting */
    HAL_AUDIO_UL_IIR_20HZ_AT_96KHZ  = 0x2,            /**< UL IIR setting */
    HAL_AUDIO_UL_IIR_50HZ_AT_96KHZ  = 0x3,            /**< UL IIR setting */
    HAL_AUDIO_UL_IIR_100HZ_AT_96KHZ = 0x4,            /**< UL IIR setting */
    HAL_AUDIO_UL_IIR_150HZ_AT_96KHZ = 0x5,            /**< UL IIR setting */
    HAL_AUDIO_UL_IIR_DUMMY          = 0xFFFFFFFF,       /**<  for DSP structrue alignment */
} hal_audio_ul_iir_t;

/** @brief Hal audio I2S data format. */
typedef enum {
    HAL_AUDIO_I2S_RJ    = 0, // Right-justified                      /**< I2S data format */
    HAL_AUDIO_I2S_LJ    = 1, // Left-justified                        /**< I2S data format */
    HAL_AUDIO_I2S_I2S   = 2,                           /**< I2S data format */
    HAL_AUDIO_I2S_DUMMY = 0xFFFFFFFF,                   /**<  for DSP structrue alignment */
} hal_audio_i2s_format_t;

/** @brief Hal audio I2S word length. */
typedef enum {
    HAL_AUDIO_I2S_WORD_LENGTH_16BIT = 0x0,              /**< I2S word length */
    HAL_AUDIO_I2S_WORD_LENGTH_32BIT = 0x1,              /**< I2S word length */
    HAL_AUDIO_I2S_WORD_LENGTH_DUMMY = 0xFFFFFFFF,       /**<  for DSP structrue alignment */
} hal_audio_i2s_word_length_t;

/** @brief Hal audio afe sample rate. */
typedef enum {
    HAL_AUDIO_FIXED_AFE_48K_SAMPLE_RATE = 48000,        /**< AFE sample rate 48K */
    HAL_AUDIO_FIXED_AFE_96K_SAMPLE_RATE = 96000,        /**< AFE sample rate 96K */
} hal_audio_afe_sample_rate_t;

/** @brief audio MCLK status structure */
typedef struct {
    bool                        status;                 /**< Audio mclk on/off status*/
    int16_t                     mclk_cntr;              /**< Audio mclk user count*/
    afe_apll_source_t           apll;                   /**< Specifies the apll of mclk source.*/
    uint8_t                     divider;                /**< Specifies the divider of mclk source, MCLK = clock_source/(1+Divider), Divider = [6:0].*/
} hal_audio_mclk_status_t;

#if defined(HAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE)
/** @brief DSP input gain selection define */
typedef enum {
    HAL_AUDIO_INPUT_GAIN_SELECTION_D0_A0     = 0, /**< Setting input digital gain0 and analog gain0 . */
    HAL_AUDIO_INPUT_GAIN_SELECTION_D0_D1     = 1, /**< Setting input digital gain0 and digital gain1 . */
    HAL_AUDIO_INPUT_GAIN_SELECTION_D2_D3     = 2, /**< Setting input digital gain2 and digital gain3 . */
    HAL_AUDIO_INPUT_GAIN_SELECTION_D4_D5     = 3, /**< Setting input digital gain4 and digital gain5 . */
    HAL_AUDIO_INPUT_GAIN_SELECTION_D6_D7     = 4, /**< Setting input digital gain6 for I2S0_L and digital gain7 for I2S0_R . */
    HAL_AUDIO_INPUT_GAIN_SELECTION_D8_D9     = 5, /**< Setting input digital gain8 for I2S1_L and digital gain9 for I2S1_R . */
    HAL_AUDIO_INPUT_GAIN_SELECTION_D10_D11   = 6, /**< Setting input digital gain10 for I2S2_L and digital gain11 for I2S2_R . */
    HAL_AUDIO_INPUT_GAIN_SELECTION_D12_D13   = 7, /**< Setting input digital gain12 for LININ_L and digital gain13 for I2S0_R . */
    HAL_AUDIO_INPUT_GAIN_SELECTION_D14       = 8, /**< Setting input digital gain14 for echo path. */
    HAL_AUDIO_INPUT_GAIN_SELECTION_D18_19    = 9,  /**< Setting input digital for mic funtion. */
    HAL_AUDIO_INPUT_GAIN_SELECTION_D20_21    = 10, /**< Setting input digital for mic funtion. */
    HAL_AUDIO_INPUT_GAIN_SELECTION_D22_23    = 11, /**< Setting input digital for mic funtion. */
    HAL_AUDIO_INPUT_GAIN_SELECTION_D24_25    = 12, /**< Setting input digital for mic funtion. */
    HAL_AUDIO_INPUT_GAIN_SELECTION_A0_A1     = 13, /**< Setting input analog gain0 and analog gain1 . */
    HAL_AUDIO_INPUT_GAIN_SELECTION_A2_A3     = 14, /**< Setting input analog gain2 and analog gain3 . */
    HAL_AUDIO_INPUT_GAIN_SELECTION_A4_A5     = 15, /**< Setting input analog gain4 and analog gain5 . */
} hal_audio_input_gain_select_t;
#endif

/**
  * @}
  */
/**
 * @}
 * @}
 */
#endif

#ifdef HAL_GPC_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPC
 * @{
 * @addtogroup hal_gpc_enum Enum
 * @{
 */
/** @brief GPC port */
typedef enum {
    HAL_GPC_0 = 0,                          /**< GPC port 0. */
    HAL_GPC_MAX_PORT                        /**< The total number of GPC ports (invalid GPC port). */
} hal_gpc_port_t;


/**
  * @}
  */
/**
 * @}
 * @}
 */
#endif


#ifdef HAL_SD_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SD
 * @{
 * @addtogroup hal_sd_enum Enum
 * @{
 */
/*****************************************************************************
* SD
*****************************************************************************/
/** @brief  This enum defines the SD/eMMC port. */
typedef enum {
    HAL_SD_PORT_0 = 0,                                             /**<  SD/eMMC port 0. */
    HAL_SD_PORT_1 = 1                                              /**<  SD/eMMC port 1. */
} hal_sd_port_t;

/**
  * @}
  */
/**
 * @}
 * @}
 */
#endif


#ifdef HAL_SDIO_SLAVE_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SDIO
 * @{
 * @addtogroup hal_sdio_enum Enum
 * @{
 */
/*****************************************************************************
* SDIO
*****************************************************************************/
/** @brief  This enum defines the SDIO port.  */
typedef enum {
    HAL_SDIO_SLAVE_PORT_0 = 0,                                             /**< SDIO slave port 0. */
} hal_sdio_slave_port_t;

/**
  * @}
  */
/**
 * @}
 * @}
 */
#endif


#ifdef HAL_SDIO_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SDIO
 * @{
 * @addtogroup hal_sdio_enum Enum
 * @{
 */
/*****************************************************************************
* SDIO
*****************************************************************************/
/** @brief  This enum defines the SDIO port.  */
typedef enum {
    HAL_SDIO_PORT_0 = 0,                                             /**< SDIO port 0. */
    HAL_SDIO_PORT_1 = 1                                              /**< SDIO port 1. */
} hal_sdio_port_t;


/**
  * @}
  */
/**
 * @}
 * @}
 */
#endif

#ifdef HAL_CLOCK_MODULE_ENABLED

/*****************************************************************************
* Clock
*****************************************************************************/

/**
 * @addtogroup HAL
 * @{
 * @addtogroup CLOCK
 * @{
 * @addtogroup hal_clock_enum Enum
 * @{
 *
 * @section CLOCK_CG_ID_Usage_Chapter HAL_CLOCK_CG_ID descriptions
 *
 * Each #hal_clock_cg_id is related to one CG. Please check the following parameters before controlling the clock.
 *
 * The description of API parameters for HAL_CLOCK_CG_ID is listed below:
 * | HAL_CLOCK_CG_ID            |Details                                                                            |
 * |----------------------------|-----------------------------------------------------------------------------------|
 * |\b HAL_CLOCK_CG_DMA         | The CG for DMA. It is controlled in DMA driver.|
 * |\b HAL_CLOCK_CG_SDIOMST_BUS | The CG for SDIO master bus. It is controlled in SDIO driver.|
 * |\b HAL_CLOCK_CG_SW_ASYS     | The CG for I2S1. It is controlled in I2S driver.|
 * |\b HAL_CLOCK_CG_SPISLV      | The CG for SPI slave. This CG should be enabled when it is connected to the master device if choosing a custom driver.|
 * |\b HAL_CLOCK_CG_SPIMST      | The CG for SPI master. It is controlled in SPI driver.|
 * |\b HAL_CLOCK_CG_SW_AUDIO    | The CG for I2S0. It is controlled in I2S driver.|
 * |\b HAL_CLOCK_CG_SDIOMST     | The CG for SDIO master. It is controlled in SDIO driver.|
 * |\b HAL_CLOCK_CG_UART1       | The CG for UART1. It is controlled in UART driver.|
 * |\b HAL_CLOCK_CG_UART2       | The CG for UART2. It is controlled in UART driver.|
 * |\b HAL_CLOCK_CG_I2C0        | The CG for I2C0. It is controlled in I2C driver.|
 * |\b HAL_CLOCK_CG_I2C1        | The CG for I2C1. It is controlled in I2C driver.|
 * |\b HAL_CLOCK_CG_CM_SYSROM   | The CG for system ROM. It cannot be disabled, otherwise the system will fail.|
 * |\b HAL_CLOCK_CG_SFC_SW      | The CG for serial flash controller. It cannot be disabled, otherwise the system will fail.|
 * |\b HAL_CLOCK_CG_SW_TRNG     | The CG for TRNG. It is controlled in TRNG driver.|
 * |\b HAL_CLOCK_CG_SW_XTALCTL  | The CG for crystal oscillator. It cannot be disabled, otherwise the system will fail.|
 * |\b HAL_CLOCK_CG_UART0       | The CG for UART0. It cannot be disabled, otherwise the system will fail.|
 * |\b HAL_CLOCK_CG_CRYPTO      | The CG for crypto engine. It is controlled in crypto engine driver.|
 * |\b HAL_CLOCK_CG_SDIOSLV     | The CG for SDIO slave. This CG should be enabled when it is connected to the master device if choosing a custom driver.|
 * |\b HAL_CLOCK_CG_PWM0        | The CG for PWM0. It is controlled in PWM driver.|
 * |\b HAL_CLOCK_CG_PWM1        | The CG for PWM1. It is controlled in PWM driver.|
 * |\b HAL_CLOCK_CG_PWM2        | The CG for PWM2. It is controlled in PWM driver.|
 * |\b HAL_CLOCK_CG_PWM3        | The CG for PWM3. It is controlled in PWM driver.|
 * |\b HAL_CLOCK_CG_PWM4        | The CG for PWM4. It is controlled in PWM driver.|
 * |\b HAL_CLOCK_CG_PWM5        | The CG for PWM5. It is controlled in PWM driver.|
 * |\b HAL_CLOCK_CG_SW_GPTIMER  | The CG for general purpose timer. It cannot be disabled, otherwise the system will fail.|
 * |\b HAL_CLOCK_CG_SW_AUXADC   | The CG for ADC. It is controlled in ADC driver.|
 */
/** @brief Use hal_clock_cg_id in Clock API. */
/*************************************************************************
 * Define clock gating registers and bit structure.
 * Note: Mandatory, modify clk_cg_mask in hal_clock.c source file, if hal_clock_cg_id has changed.
 *************************************************************************/

typedef enum {
    /* XO_PDN_PD_COND0  */
    HAL_CLOCK_CG_UART1                     =  0,        /* bit 1, PDN_COND0_FROM */
    HAL_CLOCK_CG_UART2                     =  1,        /* bit 2, PDN_COND0_FROM */
    HAL_CLOCK_CG_I2C0                      =  2,        /* bit 3, */
    HAL_CLOCK_CG_I2C1                      =  3,        /* bit 4, */
    HAL_CLOCK_CG_I2C2                      =  4,        /* bit 5, */
    HAL_CLOCK_CG_SLOW_DMA_0                =  5,        /* bit 6, */
    HAL_CLOCK_CG_SLOW_DMA_1                =  6,        /* bit 7, */
    HAL_CLOCK_CG_UART0                     =  16,        /* bit 16, */
    HAL_CLOCK_CG_AUXADC                    =  17,        /* bit 17, */

    /* XO_PDN_AO_COND0 */
    HAL_CLOCK_CG_PWM0                      = (0 + 32),    /* bit 0, XO_PDN_AO_COND0 */
    HAL_CLOCK_CG_PWM1                      = (1 + 32),    /* bit 1, */
    HAL_CLOCK_CG_PWM2                      = (2 + 32),    /* bit 2, */
    HAL_CLOCK_CG_PWM3                      = (3 + 32),    /* bit 3, */
    HAL_CLOCK_CG_PWM4                      = (4 + 32),    /* bit 4, */
    HAL_CLOCK_CG_PWM5                      = (5 + 32),    /* bit 5, */
    HAL_CLOCK_CG_PWM6                      = (6 + 32),    /* bit 6, */
    HAL_CLOCK_CG_PWM7                      = (7 + 32),    /* bit 7, */
    HAL_CLOCK_CG_PWM8                      = (8 + 32),    /* bit 8, */
    HAL_CLOCK_CG_PWM9                      = (9 + 32),    /* bit 9, */
    HAL_CLOCK_CG_SPM                       = (16 + 32),    /* bit 16, */
    HAL_CLOCK_CG_I2C_AO                    = (18 + 32),    /* bit 18, */
    HAL_CLOCK_CG_OSTIMER                   = (19 + 32),    /* bit 19, */
    HAL_CLOCK_CG_GPTIMER                   = (20 + 32),    /* bit 20, */

    /* XO_PDN_TOP_COND0 */
    HAL_CLOCK_CG_SPIMST0                   = (0 + 64),    /* bit 0, XO_PDN_TOP_COND0 */
    HAL_CLOCK_CG_SPIMST1                   = (1 + 64),    /* bit 1, XO_PDN_TOP_COND0 */
    HAL_CLOCK_CG_SPIMST2                   = (2 + 64),    /* bit 2, XO_PDN_TOP_COND0 */
    HAL_CLOCK_CG_SDIOMST                   = (3 + 64),    /* bit 2, XO_PDN_TOP_COND0 */
    HAL_CLOCK_CG_AUD_INTBUS                = (4 + 64),    /* bit 4, */
    HAL_CLOCK_CG_AUD_GPSRC                 = (5 + 64),    /* bit 5, */
    HAL_CLOCK_CG_AUD_UPLINK                = (6 + 64),    /* bit 6, */
    HAL_CLOCK_CG_AUD_DWLINK                = (7 + 64),    /* bit 7, */
    HAL_CLOCK_CG_AUD_INTF0                 = (8 + 64),   /* bit 8, */
    HAL_CLOCK_CG_AUD_INTF1                 = (9 + 64),   /* bit 9, */
    HAL_CLOCK_CG_AUD_TEST                  = (10 + 64),   /* bit 10, */
    HAL_CLOCK_CG_AUD_ANC                   = (11 + 64),   /* bit 11, */
    HAL_CLOCK_CG_DSP                       = (12 + 64),   /* bit 12, */
    HAL_CLOCK_CG_SFC                       = (17 + 64),   /* bit 17, */
    HAL_CLOCK_CG_ESC                       = (18 + 64),   /* bit 18, */
    HAL_CLOCK_CG_SPISLV                    = (19 + 64),   /* bit 19, */
    HAL_CLOCK_CG_USB                       = (20 + 64),   /* bit 20, */
    HAL_CLOCK_CG_SEJ                       = (21 + 64),   /* bit 21, */
    HAL_CLOCK_CG_MIXEDSYS                  = (22 + 64),   /* bit 22, */
    HAL_CLOCK_CG_EFUSE                     = (23 + 64),   /* bit 23, */
    HAL_CLOCK_CG_DEBUGSYS                  = (24 + 64),   /* bit 24, */

    HAL_CLOCK_CG_CM4_DMA                   = (0 + 96),    /* bit 0, PDN_PD_COND0 */
    HAL_CLOCK_CG_SPIMST0_BUS               = (1 + 96),    /* bit 1, */
    HAL_CLOCK_CG_SPIMST1_BUS               = (2 + 96),    /* bit 2, */
    HAL_CLOCK_CG_SPIMST2_BUS               = (3 + 96),    /* bit 3, */
    HAL_CLOCK_CG_AESOTF                    = (16 + 96),    /* bit 16, */
    HAL_CLOCK_CG_AESOTF_ESC                = (17 + 96),    /* bit 17, */
    HAL_CLOCK_CG_CRYPTO                    = (18 + 96),    /* bit 18, */
    HAL_CLOCK_CG_TRNG                      = (19 + 96),    /* bit 19, */
    HAL_CLOCK_CG_SPISLV_BUS                = (20 + 96),    /* bit 20, */
    HAL_CLOCK_CG_SDIOMST0                  = (21 + 96),    /* bit 21, */
    HAL_CLOCK_CG_USB_BUS                   = (22 + 96),    /* bit 22, */
    HAL_CLOCK_CG_USB_DMA                   = (23 + 96),    /* bit 23, */

    HAL_CLOCK_CG_CMSYS_ROM                 = (16 + 128), /*PDN_TOP_COND0*/
    HAL_CLOCK_CG_END                       = (17 + 128)
} hal_clock_cg_id;


/** @brief Use hal_src_clock in Clock API. */
/*************************************************************************
 * Define clock meter number.
 * Note: fill hal_src_clock into software clock meter api hal_clock_get_freq_meter as first parameter to get specific clock frequency in khz.
 *************************************************************************/
typedef enum {
    NA,
    AD_26M_DBB_1P2 = 1,        //26M/48M
    NA_1 ,
    xo_ck,                     //26M/24M
    PAD_SOC_CK,
    PAD_SOC_SRC_CK,                //NA
    AD_DCXO_CLK26M,     //26M/48M
    AD_UPLL_CLK_TEST,
    PAD_CK = 8,
    AD_APLL1_MON_CK,
    AD_APLL2_MON_CK,
    AD_UPLL_CK,
    AD_UPLL_48M_24M_CK,
    AD_OSC_CK = 13,                 //~348M
    AD_OSC_D3_CK,              //~128M
    AD_OSC_SYNC_CK,            //~8M
    AD_APLL1_CK,          //~530M
    AD_APLL2_CK,
    clk_pll1_d2,
    clk_pll1_d3,
    clk_pll1_d5,
    clk_osc1_d2 = 21,               //192M
    clk_osc1_d5,               //76.8M
    rtc_ck,                            //32k
    _hf_fsys_ck,
    clk_osc1_d3,               //76.8M
    _hf_fsfc_ck,
    _hf_fesc_ck,
    _hf_fspimst0_ck=28,
    _hf_fspimst1_ck,
    _hf_fspislv_ck,
    _hf_fsdiomst0_ck,
    _hf_fusb_ck=33,
    _hf_faud_intbus_ck,
    _hf_faud_gpsrc_ck,
    _hf_faud_uplink_ck,
    _hf_faud_dwlink_ck,
    _f_faud_intf0_ck,
    _f_faud_intf1_ck,
    _f_faud_engine_ck=40,
    _f_faud_vow_ck,
    _f_frtc_ck=44,
    _f_fxo_ck,                 //26M/24M
    f_fxo_d2_ck,               //13M/12M
    f_fosc_48m_ck,             //48M
    f_fosc_26m_ck,             //25.6M
    f_fosc_13m_ck,             //12.8M
    _hf_fdsp_ck = 52,
    hf_faud_i2s0_m_ck,
    f_f26m_ck = 62,
    f_chop_ck,
    REF_CLK
}hal_src_clock;

/** @brief Use clock_mux_sel_id in Clock API. */
/*************************************************************************
 * Define clock mux number.
 * Note: fill clock_mux_sel_id into software clock mux select api clock_mux_sel as first parameter to select specific clock domain.
 *************************************************************************/

typedef enum {
    CLK_SYS_SEL,
    CLK_DSP_SEL,
    CLK_SFC_SEL,
    CLK_ESC_SEL,
    CLK_SPIMST0_SEL,
    CLK_SDIOMST0_SEL,
    CLK_USB_SEL,
    CLK_AUD_BUS_SEL,

    CLK_AUD_GPSRC_SEL,
    CLK_AUD_ULCK_SEL,
    CLK_AUD_DLCK_SEL,
    CLK_26M_SEL,
    CLK_AUD_INTERFACE0_SEL,
    CLK_AUD_ENGINE_SEL,
    CLK_VOW_SEL,
    CLK_MCLK_SEL,
    NR_MUXS,
    CLK_SPIMST1_SEL,
    CLK_SPIMST2_SEL,
    CLK_SPISLV_SEL,
    CLK_AUD_INTERFACE1_SEL,
    CLK_PWM0_SEL = 0x20, // periferal
    CLK_PERI_NUM,
    CLK_PWM1_SEL,
    CLK_PWM2_SEL,
    CLK_PWM3_SEL,
    CLK_PWM4_SEL
} clock_mux_sel_id;






#ifdef HAL_DVFS_MODULE_ENABLED
#ifdef AB1568
/** @brief  This macro define the lowest vcore voltage value of platform.  */
#define MTK_DVFS_LOWV_OP75V                                  /*The lowest voltage option. */
/** @brief  This macro define the lowest vcore voltage description of platform.  */
#define HAL_DVFS_VCORE_LOWV        PMIC_VCORE_0P75_V         /*The pmu lowest voltage description. */
/** @brief  This macro define the test vcore voltage description of platform.  */
#define HAL_DVFS_VCORE_TEST_LOWV   PMIC_VCORE_0P73_V         /*The pmu test voltage description. */
#endif
#ifdef AB1565
/** @brief  This macro define the lowest vcore voltage value of platform.  */
#define MTK_DVFS_LOWV_OP8V
/** @brief  This macro define the lowest vcore voltage description of platform.  */
#define HAL_DVFS_VCORE_LOWV        PMIC_VCORE_0P8_V          /*The pmu lowest voltage description. */
/** @brief  This macro define the test vcore voltage description of platform.  */
#define HAL_DVFS_VCORE_TEST_LOWV   PMIC_VCORE_0P8_V          /*The pmu test voltage description. */
#endif

/** @brief Use dvfs_frequency_t in DVFS API. */
/*************************************************************************
 * Define dvfs level number.
 * Note: fill dvfs_frequency_t into dvfs level lock control api hal_dvfs_lock_contorl as first parameter
   to specify a dvfs level as minimum speed.
 *************************************************************************/
typedef enum {
    HAL_DVFS_HALF_SPEED_52M_W_LDSP= 0,
    HAL_DVFS_HALF_SPEED_52M ,
    HAL_DVFS_FULL_SPEED_104M,
    HAL_DVFS_HIGH_SPEED_208M,
    HAL_DVFS_MAX_SPEED,
} dvfs_frequency_t;
#endif
/**
  * @}
  */

/**
 * @}
 * @}
 */
#endif

#ifdef HAL_SW_DMA_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SW_DMA
 * @{
 * @defgroup hal_sw_dma_define Define
 * @{
 */

/** @brief  The maximum number of SW DMA users.
 */
#define    SW_DMA_USER_NUM  (8)

/**
 * @}
 */


/**
 * @}
 * @}
 */
#endif

#ifdef __cplusplus
}
#endif

#endif /* __HAL_PLATFORM_H__ */

