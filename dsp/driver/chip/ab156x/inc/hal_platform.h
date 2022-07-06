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
#ifdef HAL_CACHE_MODULE_ENABLED
#define HAL_CACHE_WITH_REMAP_FEATURE     /* Enable CACHE setting with remap feature. */
#define HAL_CACHE_REGION_CONVERT         /* Enable mutual conversion between cacheable and non-cacheable addresses*/
#endif

#ifdef HAL_AUDIO_MODULE_ENABLED
#define HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT
#define HAL_AUDIO_ENABLE_PATH_MEM_DEVICE /* Enable audio path, momory,device driver*/
#define MTK_AUDIO_HW_IO_CONFIG_ENHANCE /* Enable audio HW IO config enhange*/

#endif

#ifdef HAL_I2C_MASTER_MODULE_ENABLED
#define HAL_I2C_MASTER_FEATURE_SEND_TO_RECEIVE  /*Enable I2C master send to receive feature. */
#define HAL_I2C_MASTER_FEATURE_EXTENDED_DMA    /* Enable I2C master extend DMA feature.*/

#endif

#ifdef HAL_SPI_MASTER_MODULE_ENABLED
#define HAL_SPI_MASTER_FEATURE_ADVANCED_CONFIG       /* Enable SPI master advanced configuration feature. For more details, please refer to hal_spi_master.h.*/
#define HAL_SPI_MASTER_FEATURE_DEASSERT_CONFIG       /* Enable SPI master deassert configuration feature to deassert the chip select signal after each byte data transfer is complete. For more details, please refer to hal_spi_master.h.*/
#define HAL_SPI_MASTER_FEATURE_CHIP_SELECT_TIMING    /* Enable SPI master chip select timing configuration feature to set timing value for chip select signal. For more details, please refer to hal_spi_master.h.*/
#define HAL_SPI_MASTER_FEATURE_DMA_MODE              /* Enable SPI master DMA mode feature to do data transfer. For more details, please refer to hal_spi_master.h.*/
#define HAL_SPI_MASTER_FEATURE_DUAL_QUAD_MODE        /* Enable SPI master to use dual mode and quad mode. For more details, please refer to hal_spi_master.h. */
#endif

#ifdef HAL_GPIO_MODULE_ENABLED
#define HAL_GPIO_FEATURE_PUPD               /* Pull state of the pin can be configured with different resistors through different combinations of GPIO_PUPD_x,GPIO_RESEN0_x and GPIO_RESEN1_x. For more details, please refer to hal_gpio.h. */
// #define HAL_GPIO_FEATURE_CLOCKOUT           /* The pin can be configured as an output clock. For more details, please refer to hal_gpio.h. */
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


#ifdef HAL_GPT_MODULE_ENABLED
#define HAL_GPT_FEATURE_US_TIMER               /* Supports a microsecond timer. */
#define HAL_GPT_SW_GPT_FEATURE                 /* Supports software GPT timer. */
#define HAL_GPT_PORT_ALLOCATE                  /* Allocates GPT communication port. */
#define HAL_GPT_SW_GPT_US_FEATURE              /* Supports software GPT us timer. */
#define HAL_GPT_SW_FEATURE_ABSOLUTE_COUNT       /* Supports software GPT absolute count in softeware GPT timeline */
#endif

#ifdef HAL_I2S_MODULE_ENABLED
#define HAL_I2S_EXTENDED                        /* Supports advanced settings. */
#define HAL_I2S_SUPPORT_VFIFO                   /* Supports virtual FIFO. */
#define HAL_I2S_FEATURE_MULTI_I2S               /* Supports multiple I2S connections. */
#define HAL_I2S_FEATURE_TDM                     /* Supports TDM mode. */
#endif

#ifdef HAL_UART_MODULE_ENABLED
#define HAL_UART_FEATURE_VFIFO_DMA_TIMEOUT        /* Supports configurable timeout value setting */
#define HAL_UART_FEATURE_3M_BAUDRATE              /* Supports UART 3M baudrate setting */
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
 * |  UART2    |           V           |
 * |  UART3    |           X           |
 */
typedef enum {
    HAL_UART_0 = 0,                            /**< UART port 0. */
    HAL_UART_1 = 1,                            /**< UART port 1. */
    HAL_UART_2 = 2,                            /**< UART port 2. Only polling mode is available. */
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
#define HAL_I2C_MAXIMUM_DMA_TRANSACTION_SIZE   15

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
 *  The platform supports 4 I2C masters. Two of them support polling mode and DMA mode,
 *  while the others only supports polling mode. For more information about the polling mode,
 *  DMA mode, queue mode, please refer to @ref HAL_I2C_Features_Chapter. The details
 *  are shown below:
 *  - Supported features of I2C masters \n
 *    V : supported;  X : not supported.
 * |I2C Master   | Polling mode | DMA mode | Extended DMA mode |
 * |-------------|--------------|----------|-------------------|
 * |I2C0         |      V       |    V     |         V         |
 * |I2C1         |      V       |    V     |         V         |
 * |I2C2         |      V       |    X     |         X         |
 * |I2CAO        |      V       |    X     |         X         |
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
    HAL_GPIO_20 = 20,   /**< GPIO pin20. */
    HAL_GPIO_21 = 21,   /**< GPIO pin21. */
    HAL_GPIO_22 = 22,   /**< GPIO pin22. */
    HAL_GPIO_23 = 23,   /**< GPIO pin23. */
    HAL_GPIO_24 = 24,   /**< GPIO pin24. */
    HAL_GPIO_25 = 25,   /**< GPIO pin25. */
    HAL_GPIO_26 = 26,   /**< GPIO pin26. */
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
    HAL_GPIO_CLOCK_4   = 4,              /**< define GPIO output clock 4 */
    HAL_GPIO_CLOCK_5   = 5,              /**< define GPIO output clock 5 */
    HAL_GPIO_CLOCK_MAX                   /**< define GPIO output clock max number(invalid) */
} hal_gpio_clock_t;


/** @brief This enum defines output clock mode of GPIO */
typedef enum {
    HAL_GPIO_CLOCK_MODE_32K = 0,        /**< define GPIO output clock mode as 32KHz */
    HAL_GPIO_CLOCK_MODE_24M = 2,        /**< define GPIO output clock mode as 24MHz */
    HAL_GPIO_CLOCK_MODE_48M = 3,        /**< define GPIO output clock mode as 48MHz */
    HAL_GPIO_CLOCK_MODE_40K = 5,        /**< define GPIO output clock mode as 40MHz */
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
    HAL_SPI_MASTER_MAX                                 /**< The total number of SPI master ports (invalid SPI master port). */
} hal_spi_master_port_t;

/** @brief This enum defines the options to connect the SPI slave device to the SPI master's CS pins. */
typedef enum {
    HAL_SPI_MASTER_SLAVE_0 = 0,                       /**< The SPI slave device is connected to the SPI master's CS0 pin. */
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
    HAL_EINT_NUMBER_20 = 20,
    HAL_EINT_NUMBER_21 = 21,
    HAL_EINT_NUMBER_22 = 22,
    HAL_EINT_NUMBER_23 = 23,
    HAL_EINT_NUMBER_24 = 24,
    HAL_EINT_NUMBER_25 = 25,
    HAL_EINT_NUMBER_26 = 26,
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
/* NULL */

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
    HAL_PWM_5 = 5,                            /**< PWM channel 5. */
    HAL_PWM_MAX_CHANNEL                     /**< The total number of PWM channels (invalid PWM channel).*/
} hal_pwm_channel_t;


/** @brief PWM clock source options */
typedef enum {
    HAL_PWM_CLOCK_13MHZ = 0,                /**< PWM clock source 13MHz. */
    HAL_PWM_CLOCK_32KHZ = 1,                /**< PWM clock srouce 32kHz. */
    HAL_PWM_CLOCK_40MHZ = 2,                /**< PWM clock srouce 40MHz. */
} hal_pwm_source_clock_t ;

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
/**
 * @brief  This macro defines the minimized CACHE line size, if i_cache line size equal to d_cache line size.
 *         We do not command using this macro. Recommended use HAL_I_CACHE_LINE_SIZE and HAL_D_CACHE_LINE_SIZE,respectively.
 */
#define HAL_CACHE_LINE_SIZE         (16)

/**
 * @brief  This macro defines the I_CACHE line size.
 */
#define HAL_I_CACHE_LINE_SIZE         (64)

/**
 * @brief  This macro defines the D_CACHE line size.
 */
#define HAL_D_CACHE_LINE_SIZE         (32)

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



/*****************************************************************************
* Audio setting
*****************************************************************************/
#if 0//modify for ab1568
/** @brief Audio device. */
typedef enum {
    HAL_AUDIO_DEVICE_NONE               = 0x0000,  /**<  No audio device is on. */
    HAL_AUDIO_DEVICE_MAIN_MIC_L         = 0x0001,  /**<  Stream in: main mic L. */
    HAL_AUDIO_DEVICE_MAIN_MIC_R         = 0x0002,  /**<  Stream in: main mic R. */
    HAL_AUDIO_DEVICE_MAIN_MIC_DUAL      = 0x0003,  /**<  Stream in: main mic L+R. */
    HAL_AUDIO_DEVICE_LINEINPLAYBACK_L   = 0x0004,  /**<  Stream in: line in playback L. */
    HAL_AUDIO_DEVICE_LINEINPLAYBACK_R   = 0x0008,  /**<  Stream in: line in playback R. */
    HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL= 0x000c,  /**<  Stream in: line in playback L+R. */
    HAL_AUDIO_DEVICE_DIGITAL_MIC_L      = 0x0010,  /**<  Stream in: digital mic L. */
    HAL_AUDIO_DEVICE_DIGITAL_MIC_R      = 0x0020,  /**<  Stream in: digital mic R. */
    HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL   = 0x0030,  /**<  Stream in: digital mic L+R. */

    HAL_AUDIO_DEVICE_DAC_L              = 0x0100,  /**<  Stream out:speaker L. */
    HAL_AUDIO_DEVICE_DAC_R              = 0x0200,  /**<  Stream out:speaker R. */
    HAL_AUDIO_DEVICE_DAC_DUAL           = 0x0300,  /**<  Stream out:speaker L+R. */

    HAL_AUDIO_DEVICE_I2S_MASTER         = 0x1000,  /**<  Stream in/out: I2S master role */
    HAL_AUDIO_DEVICE_I2S_SLAVE          = 0x2000,  /**<  Stream in/out: I2S slave role */
    HAL_AUDIO_DEVICE_EXT_CODEC          = 0x3000,   /**<  Stream out: external amp.&codec, stereo/mono */

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
/** @brief audio channel selection define */
typedef enum {
    HAL_AUDIO_DIRECT                     = 0, /**< A single interconnection, output equal to input. */
    HAL_AUDIO_SWAP_L_R                   = 2, /**< L and R channels are swapped. That is (L, R) -> (R, L). */
    HAL_AUDIO_BOTH_L                     = 3, /**< only output L channel. That is (L, R) -> (L, L). */
    HAL_AUDIO_BOTH_R                     = 4, /**< only output R channel. That is (L, R) -> (R, R). */
    HAL_AUDIO_MIX_L_R                    = 5, /**< L and R channels are mixed. That is (L, R) -> (L+R, L+R). */
    HAL_AUDIO_MIX_SHIFT_L_R              = 6, /**< L and R channels are mixed and shift. That is (L, R) -> (L/2+R/2, L/2+R/2). */
    HAL_AUDIO_CHANNEL_DUMMY              = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} hal_audio_channel_selection_t;

typedef enum {
    HAL_AUDIO_MEM1                              = 0x0001,       /**< Memory path 1. UL:DL1_data, VUL:VUL1_data   */
    HAL_AUDIO_MEM2                              = 0x0002,       /**< Memory path 2. UL:DL2_data, VUL:VUL2_data   */
    HAL_AUDIO_MEM3                              = 0x0004,       /**< Memory path 3. UL:DL3_data, VUL:AWB_data   */
    HAL_AUDIO_MEM4                              = 0x0008,       /**< Memory path 4. UL:DL12_data, VUL:AWB2_data   */
    HAL_AUDIO_MEM5                              = 0x0010,       /**< Memory path 5. VUL:VUL3_data   */
    HAL_AUDIO_MEM6                              = 0x0020,       /**< Memory path 6. SLAVE DMA data   */
    HAL_AUDIO_MEM7                              = 0x0040,       /**< Memory path 7. SLAVE TDM data   */
    HAL_AUDIO_MEM_SUB                           = 0x0100,       /**< Memory path Sub. memory interfaceusage depend on mian source   */
    HAL_AUDIO_MEM_DUMMY                         = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} hal_audio_memory_t;

/** @brief audio interface define */
typedef enum {
    HAL_AUDIO_INTERFACE_NONE                    = 0x0000,       /**< No audio interface should be selected, */
    HAL_AUDIO_INTERFACE_1                       = 0x0001,       /**< Audio interface path 1. UL:UL SRC ch1 ch2, I2S Master:I2S0, */
    HAL_AUDIO_INTERFACE_2                       = 0x0002,       /**< Audio interface path 2. UL:UL SRC ch3 ch4, I2S Master:I2S1, */
    HAL_AUDIO_INTERFACE_3                       = 0x0004,       /**< Audio interface path 3. UL:UL SRC ch5 ch6, I2S Master:I2S2, */
    HAL_AUDIO_INTERFACE_4                       = 0x0008,       /**< Audio interface path 4. I2S Master:I2S3, */
    HAL_AUDIO_INTERFACE_DUMMY                   = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} hal_audio_interface_t;

/** @brief i2s clk source define */
typedef enum {
    I2S_CLK_SOURCE_APLL                         = 0, /**< Low jitter mode. */
    I2S_CLK_SOURCE_DCXO                         = 1, /**< Normal mode. */
    I2S_CLK_SOURCE_TYPE_DUMMY                   = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} I2S_CLK_SOURCE_TYPE;

/** @brief micbias source define */
typedef enum {
    MICBIAS_0                                   = 0x1, /**< Open micbias0. */
    MICBIAS_1                                   = 0x2, /**< Open micbias1. */
    MICBIAS_ALL                                 = 0x3, /**< Open micbias0 and micbias1. */
    MICBIAS_SOURCE_TYPE_DUMMY                   = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} MICBIAS_SOURCE_TYPE;

/** @brief audio dmic clock selection define */
typedef enum {
    HAL_AUDIO_DMIC_CLOCK_0                      = 0,    /**<Select clock to DMic0, intercom I8&I9. */
    HAL_AUDIO_DMIC_CLOCK_1                      = 1,    /**<Select clock to DMic1, intercom I10&I11. */
    HAL_AUDIO_DMIC_CLOCK_2                      = 2,    /**<Select clock to DMic2, intercom I12&I13. */
    HAL_AUDIO_DMIC_CLOCK_ANC                    = 3,    /**<Select clock to DMic ANC */
} hal_audio_dmic_clock_selection_t;

/** @brief amp performance define */
typedef enum {
    AUDIO_AMP_PERFORMANCE_NORMAL                = 0, /**< Normal mode. */
    AUDIO_AMP_PERFORMANCE_HIGH                  = 1, /**< High performance mode. */
    AUDIO_AMP_PERFORMANCE_TYPE_DUMMY            = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} AUDIO_AMP_PERFORMANCE_TYPE;

/** @brief DSP streaming source channel define */
typedef enum {
    AUDIO_DSP_CHANNEL_SELECTION_STEREO          = 0, /**< DSP streaming output L and R will be it own. */
    AUDIO_DSP_CHANNEL_SELECTION_MONO            = 1, /**< DSP streaming output L and R will be (L+R)/2. */
    AUDIO_DSP_CHANNEL_SELECTION_BOTH_L          = 2, /**< DSP streaming output L and R will be both L. */
    AUDIO_DSP_CHANNEL_SELECTION_BOTH_R          = 3, /**< DSP streaming output L and R will be both R. */
    AUDIO_DSP_CHANNEL_SELECTION_NUM,
#ifdef ENABLE_2A2D_TEST
    AUDIO_DSP_CHANNEL_SELECTION_REF_MIC_3       = 4, /**< DSP streaming output L and R will be both R. */
    AUDIO_DSP_CHANNEL_SELECTION_REF_MIC_4       = 5, /**< DSP streaming output L and R will be both R. */
    AUDIO_DSP_CHANNEL_SELECTION_REF_MIC_5       = 6, /**< DSP streaming output L and R will be both R. */
    AUDIO_DSP_CHANNEL_SELECTION_REF_MIC_6       = 7, /**< DSP streaming output L and R will be both R. */
#endif
} AUDIO_DSP_CHANNEL_SELECTION;

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


/** @brief DSP output gain selection define */
typedef enum {
    HAL_AUDIO_OUTPUT_GAIN_SELECTION_D0_ALR    = 0,  /**< Setting output digital gain0 and analog gain L&R . */
    HAL_AUDIO_OUTPUT_GAIN_SELECTION_D1_ALR    = 1,  /**< Setting output digital gain1 and analog gain L&R . */
    HAL_AUDIO_OUTPUT_GAIN_SELECTION_D2_ALR    = 2,  /**< Setting output digital gain2 and analog gain L&R . */
    HAL_AUDIO_OUTPUT_GAIN_SELECTION_AOL_AOR   = 16, /**< Setting output analog offset gain L and offset gain R. */
} hal_audio_output_gain_select_t;

#ifdef __cplusplus
}
#endif

#endif /* __HAL_PLATFORM_H__ */

