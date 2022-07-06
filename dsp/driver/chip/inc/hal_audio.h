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

#ifndef __HAL_AUDIO_H__
#define __HAL_AUDIO_H__
#include "hal_platform.h"

#ifdef HAL_AUDIO_MODULE_ENABLED

#include "hal_ccni.h"
#include "types.h"
#ifdef HAL_AUDIO_ENABLE_PATH_MEM_DEVICE
#include "hal_audio_common.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif

#include "hal_gpio.h"
#include "hal_pdma_internal.h"
/** @defgroup hal audio control define
  * @{
  */

/**
 * @brief  This macro defines the hal audio gpio setting.
 */
#define HAL_AUDIO_GPIO_STATUS_HIGH                          (HAL_GPIO_DATA_HIGH)
#define HAL_AUDIO_GPIO_STATUS_LOW                           (HAL_GPIO_DATA_LOW)
#define HAL_AUDIO_GPIO_PINMUX_GPIO_MODE                     (0)
#define HAL_AUDIO_GPIO_INIT(gpio, pinmux)                   hal_gpio_init(gpio); \
                                                            hal_pinmux_set_function(gpio, pinmux); \
                                                            hal_gpio_set_direction(gpio, HAL_GPIO_DIRECTION_OUTPUT)
#define HAL_AUDIO_GPIO_SET_STATUS(gpio, status)             hal_gpio_set_output(gpio, status)


/**
 * @brief  This macro defines the dsp audio sync type.
 */
//#ifndef _DSP_CONTROL_H_
#define DSP_AU_SYNC_NONE                                    (0x0001)
#define DSP_AU_SYNC_SW_TRIGGER                              (0x0002)
#define DSP_AU_SYNC_AUDIO_FORWARDER                         (0x0004)
#define DSP_AU_SYNC_PLAY_EN                                 (0x0008)
#define DSP_AU_SYNC_MULTIPLE_CHANNEl                        (0x0010)
//#endif

/**
 * @brief  This macro defines the audio path setting.
 */

/* Path */
#define HAL_AUDIO_PATH_SUPPORT_SEQUENCE                     (8)
#define HAL_AUDIO_PATH_ECHO_CONNECTION_MODE                 (0) //0: Connect from interconn, 1:Connect from UL loopback

/**
 * @brief  This macro defines the audio analog gain index.
 */

/* Volume */
#define HAL_AUDIO_INVALID_ANALOG_GAIN_INDEX                 (0x7FFF)

/**
 * @brief  This macro defines the vow fifo copy size.
 */
/*VOW*/
#define VOW_SRAM_COPY_SIZE  (128)  //Units:sample

//modify for ab1568
/**
 * @brief  This macro defines the function of the array size.
 */
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define OS_ASSERT(x) assert(x)

/**
  * @}
  */


/** @defgroup hal_audio_enum Enum
  * @{
  */

/** @brief Audio status. */
typedef enum {
    HAL_AUDIO_STATUS_INVALID_PARAMETER  = -2,  /**<  A wrong parameter is given. */
    HAL_AUDIO_STATUS_ERROR              = -1,  /**<  An error occured during the function call. */
    HAL_AUDIO_STATUS_OK                 =  0,  /**<  No error occurred during the function call. */
} hal_audio_status_t;

/** @brief Audio control. */
typedef enum {
    HAL_AUDIO_CONTROL_OFF                =  0,  /**<  Turn off. */
    HAL_AUDIO_CONTROL_ON                 =  1,  /**<  Turn on. */
} hal_audio_control_status_t;



/** @brief Audio control. */
typedef enum {
    HAL_AUDIO_CONTROL_NONE                              = 0x0000,   /**<  No audio device is on. */

    HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_L               = 0x0001,
    HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_R               = 0x0002,
    HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL            = 0x0003,   /**<  Stream in:     main mic. */

    HAL_AUDIO_CONTROL_DEVICE_LINE_IN_L                  = 0x0004,
    HAL_AUDIO_CONTROL_DEVICE_LINE_IN_R                  = 0x0008,
    HAL_AUDIO_CONTROL_DEVICE_LINE_IN_DUAL               = 0x000C,   /**<  Stream in:     LineIn. */

    HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_L              = 0x0010,
    HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_R              = 0x0020,
    HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL           = 0x0030,   /**<  Stream in:     digital mic. */

    HAL_AUDIO_CONTROL_DEVICE_ANC                        = 0x0040,   /**<  Stream in:     HW ANC. */
    HAL_AUDIO_CONTROL_DEVICE_VAD                        = 0x0080,   /**<  Stream in:     HW VAD. */
    HAL_AUDIO_CONTROL_DEVICE_VOW                        = 0x00C0,   /**<  Stream in:     VOW. */

    HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_L             = 0x0100,
    HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_R             = 0x0200,
    HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_DUAL          = 0x0300,   /**<  Stream out:    AMP. */

    HAL_AUDIO_CONTROL_DEVICE_SIDETONE                   = 0x0400,   /**<  Stream out:    HW Sidetone. */
    HAL_AUDIO_CONTROL_DEVICE_LOOPBACK                   = 0x0800,   /**<  Stream in:     HW AD loopback. */

    HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER                 = 0x1000,   /**<  Stream in/out: I2S master. */
    HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE                  = 0x2000,   /**<  Stream in/out: I2S slave. */
    HAL_AUDIO_CONTROL_DEVICE_SPDIF                      = 0x4000,   /**<  Stream out: SPDIF. */

    HAL_AUDIO_CONTROL_MEMORY_INTERFACE                  = 0x8000,   /**<  Stream internal: Memory interface. */
    HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_L               = 0x10000,
    HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_R               = 0x20000,
    HAL_AUDIO_CONTROL_DUMMY                             = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} hal_audio_control_t;

/**
 * @brief  This macro defines the hal_audio_control remapping.
 */
#ifdef HAL_AUDIO_ENABLE_PATH_MEM_DEVICE
#define hal_audio_device_t hal_audio_control_t
#endif

/** @brief hal audio device interface */
typedef enum {
    HAL_AUDIO_CONTROL_DEVICE_INTERFACE_NONE             = 0x0000,       /**< No audio interface should be selected, */
    HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1                = 0x0001,       /**< Audio interface path 1. UL:UL SRC ch1 ch2, I2S Master:I2S0, I2S Slave:I2S0 */
    HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2                = 0x0002,       /**< Audio interface path 2. UL:UL SRC ch3 ch4, I2S Master:I2S1, I2S Slave:I2S1 */
    HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3                = 0x0004,       /**< Audio interface path 3. UL:UL SRC ch5 ch6, I2S Master:I2S2, I2S Slave:I2S2 */
    HAL_AUDIO_CONTROL_DEVICE_INTERFACE_4                = 0x0008,       /**< Audio interface path 4. UL:UL SRC ch7 ch8, */
    HAL_AUDIO_CONTROL_DEVICE_INTERFACE_DUMMY            = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} hal_audio_device_interface_t;

/** @brief audio memory selection */
typedef enum {
    HAL_AUDIO_MEMORY_DL_DL1                             = 0x0001,       /**< Memory path DL:DL1_data    */
    HAL_AUDIO_MEMORY_DL_DL2                             = 0x0002,       /**< Memory path DL:DL2_data    */
    HAL_AUDIO_MEMORY_DL_DL3                             = 0x0004,       /**< Memory path DL:DL3_data    */
    HAL_AUDIO_MEMORY_DL_DL12                            = 0x0008,       /**< Memory path DL:DL12_data   */
    HAL_AUDIO_MEMORY_DL_SRC1                            = 0x0010,       /**< Memory path DL:SRC1 data   */
    HAL_AUDIO_MEMORY_DL_SRC2                            = 0x0020,       /**< Memory path DL:SRC2 data   */
    HAL_AUDIO_MEMORY_DL_SLAVE_DMA                       = 0x0040,       /**< Memory path DL:SLAVE DMA data   */
    HAL_AUDIO_MEMORY_DL_SLAVE_TDM                       = 0x0080,       /**< Memory path DL:SLAVE TDM data   */

    HAL_AUDIO_MEMORY_UL_VUL1                            = 0x0100,       /**< Memory path UL:VUL1_data   */
    HAL_AUDIO_MEMORY_UL_VUL2                            = 0x0200,       /**< Memory path UL:VUL2_data   */
    HAL_AUDIO_MEMORY_UL_VUL3                            = 0x0400,       /**< Memory path UL:VUL3_data   */
    HAL_AUDIO_MEMORY_UL_AWB                             = 0x0800,       /**< Memory path UL:AWB_data    */
    HAL_AUDIO_MEMORY_UL_AWB2                            = 0x1000,       /**< Memory path UL:AWB2_data   */
    HAL_AUDIO_MEMORY_UL_SLAVE_DMA                       = 0x2000,       /**< Memory path UL:SLAVE DMA data   */
    HAL_AUDIO_MEMORY_UL_SLAVE_TDM                       = 0x4000,       /**< Memory path UL:SLAVE TDM data   */

    HAL_AUDIO_MEMORY_DL_MASK                            = 0x00FF,
    HAL_AUDIO_MEMORY_UL_MASK                            = 0x7F00,

    HAL_AUDIO_MEMORY_POWER_DETECTOR_L                   = 0x10000,
    HAL_AUDIO_MEMORY_POWER_DETECTOR_R                   = 0x20000,
    HAL_AUDIO_MEMORY_POWER_DETECTOR_MASK                = 0x30000,

    HAL_AUDIO_MEMORY_DUMMY                              = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} hal_audio_memory_selection_t;


/** @brief audio memory sync selection, Align DSP_AU_SYNC_CTRL_t */
typedef enum {
    HAL_AUDIO_MEMORY_SYNC_NONE                  = DSP_AU_SYNC_NONE,                 /**< 0x0000   DSP_AU_SYNC_NONE              */
    HAL_AUDIO_MEMORY_SYNC_SW_TRIGGER            = DSP_AU_SYNC_SW_TRIGGER,           /**< 0x0001   DSP_AU_SYNC_SW_TRIGGER        */
    HAL_AUDIO_MEMORY_SYNC_AUDIO_FORWARDER       = DSP_AU_SYNC_AUDIO_FORWARDER,      /**< 0x0002   DSP_AU_SYNC_AUDIO_FORWARDER        */
    HAL_AUDIO_MEMORY_SYNC_PLAY_EN               = DSP_AU_SYNC_PLAY_EN,              /**< 0x0004   DSP_AU_SYNC_PLAY_EN           */
    HAL_AUDIO_MEMORY_SYNC_MULTIPLE_CHANNEl      = DSP_AU_SYNC_MULTIPLE_CHANNEl,     /**< 0x0010   DSP_AU_SYNC_MULTIPLE_CHANNEl  */
    HAL_AUDIO_MEMORY_SYNC_DUMMY                 = 0xFFFFFFFF,                       /**<  for DSP structrue alignment */
} hal_audio_memory_sync_selection_t;


/** @brief audio interconn channel selection */
typedef enum {

    HAL_AUDIO_INTERCONN_CH01CH02_to_CH01CH02 = 0,       /**< Stereo, ch01->ch01, ch02->ch02. */
    HAL_AUDIO_INTERCONN_CH01CH02_to_CH02CH01,           /**< Stereo, ch01->ch02, ch02->ch01. */
    HAL_AUDIO_INTERCONN_MIX,                            /**< Stereo, ch01->ch01&ch02, ch02->ch01&ch02. */
    HAL_AUDIO_INTERCONN_MIX_SHIFT_RIGHT,

    HAL_AUDIO_INTERCONN_CH01_to_CH01CH02,
    HAL_AUDIO_INTERCONN_CH01_to_CH01,
    HAL_AUDIO_INTERCONN_CH01_to_CH02,

    HAL_AUDIO_INTERCONN_CH02_to_CH01CH02,
    HAL_AUDIO_INTERCONN_CH02_to_CH01,
    HAL_AUDIO_INTERCONN_CH02_to_CH02,

    HAL_AUDIO_INTERCONN_NUMBER,
    HAL_AUDIO_INTERCONN_CHANNEL_DUMMY                   = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} hal_audio_interconn_channel_selection_t;

typedef uint8_t hal_audio_interconn_sequence_t;
/** @brief audio interconn sequence selection */
typedef enum {
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MIN                       = (hal_audio_interconn_sequence_t) 0,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_DAC_CH1            = HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MIN,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_DAC_CH2            = (hal_audio_interconn_sequence_t) 1,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_MASTER_I2S0_CH1    = (hal_audio_interconn_sequence_t) 2,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_MASTER_I2S0_CH2    = (hal_audio_interconn_sequence_t) 3,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_MASTER_I2S1_CH1    = (hal_audio_interconn_sequence_t) 4,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_MASTER_I2S1_CH2    = (hal_audio_interconn_sequence_t) 5,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_MASTER_I2S2_CH1    = (hal_audio_interconn_sequence_t) 6,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_MASTER_I2S2_CH2    = (hal_audio_interconn_sequence_t) 7,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_SLAVE_I2S0_CH1     = (hal_audio_interconn_sequence_t) 8,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_SLAVE_I2S0_CH2     = (hal_audio_interconn_sequence_t) 9,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_SLAVE_I2S1_CH1     = (hal_audio_interconn_sequence_t)10,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_SLAVE_I2S1_CH2     = (hal_audio_interconn_sequence_t)11,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_SLAVE_I2S2_CH1     = (hal_audio_interconn_sequence_t)12,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_SLAVE_I2S2_CH2     = (hal_audio_interconn_sequence_t)13,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_SIDETONE_CH1       = (hal_audio_interconn_sequence_t)14,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_SIDETONE_CH2       = (hal_audio_interconn_sequence_t)15,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_MIN                = (hal_audio_interconn_sequence_t)16,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL1_CH1           = HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_MIN,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL1_CH2           = (hal_audio_interconn_sequence_t)17,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL2_CH1           = (hal_audio_interconn_sequence_t)18,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL2_CH2           = (hal_audio_interconn_sequence_t)19,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL3_CH1           = (hal_audio_interconn_sequence_t)20,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL3_CH2           = (hal_audio_interconn_sequence_t)21,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_AWB_CH1            = (hal_audio_interconn_sequence_t)22,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_AWB_CH2            = (hal_audio_interconn_sequence_t)23,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_AWB2_CH1           = (hal_audio_interconn_sequence_t)24,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_AWB2_CH2           = (hal_audio_interconn_sequence_t)25,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_MAX                = HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_AWB2_CH2,
    HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MAX                       = HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_AWB2_CH2,

    HAL_AUDIO_INTERCONN_SELECT_INPUT_MIN                        = (hal_audio_interconn_sequence_t)32,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL1_CH1             = HAL_AUDIO_INTERCONN_SELECT_INPUT_MIN,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL1_CH2             = (hal_audio_interconn_sequence_t)33,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL2_CH1             = (hal_audio_interconn_sequence_t)34,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL2_CH2             = (hal_audio_interconn_sequence_t)35,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL3_CH1             = (hal_audio_interconn_sequence_t)36,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL3_CH2             = (hal_audio_interconn_sequence_t)37,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL4_CH1             = (hal_audio_interconn_sequence_t)38,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL4_CH2             = (hal_audio_interconn_sequence_t)39,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_MASTER_I2S0_CH1     = (hal_audio_interconn_sequence_t)40,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_MASTER_I2S0_CH2     = (hal_audio_interconn_sequence_t)41,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_MASTER_I2S1_CH1     = (hal_audio_interconn_sequence_t)42,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_MASTER_I2S1_CH2     = (hal_audio_interconn_sequence_t)43,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_MASTER_I2S2_CH1     = (hal_audio_interconn_sequence_t)44,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_MASTER_I2S2_CH2     = (hal_audio_interconn_sequence_t)45,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_SLAVE_I2S0_CH1      = (hal_audio_interconn_sequence_t)46,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_SLAVE_I2S0_CH2      = (hal_audio_interconn_sequence_t)47,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_SLAVE_I2S1_CH1      = (hal_audio_interconn_sequence_t)48,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_SLAVE_I2S1_CH2      = (hal_audio_interconn_sequence_t)49,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_SLAVE_I2S2_CH1      = (hal_audio_interconn_sequence_t)50,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_SLAVE_I2S2_CH2      = (hal_audio_interconn_sequence_t)51,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_MIN                 = (hal_audio_interconn_sequence_t)52,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL1_CH1             = HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_MIN,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL1_CH2             = (hal_audio_interconn_sequence_t)53,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL2_CH1             = (hal_audio_interconn_sequence_t)54,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL2_CH2             = (hal_audio_interconn_sequence_t)55,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL3_CH1             = (hal_audio_interconn_sequence_t)56,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL3_CH2             = (hal_audio_interconn_sequence_t)57,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL12_CH1            = (hal_audio_interconn_sequence_t)58,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL12_CH2            = (hal_audio_interconn_sequence_t)59,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_SRC1_CH1            = (hal_audio_interconn_sequence_t)60,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_SRC1_CH2            = (hal_audio_interconn_sequence_t)61,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_SRC2_CH1            = (hal_audio_interconn_sequence_t)62,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_SRC2_CH2            = (hal_audio_interconn_sequence_t)63,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_MAX                 = HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_SRC2_CH2,
    HAL_AUDIO_INTERCONN_SELECT_INPUT_MAX                        = HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_SRC2_CH2,


    HAL_AUDIO_INTERCONN_SEQUENCE_DUMMY                          = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} hal_audio_interconn_selection_t;




/** @briefaudio interconn channel selection */
typedef enum {
    HAL_AUDIO_PCM_FORMAT_S8 = 0,                       /**< PMC format setting  */
    HAL_AUDIO_PCM_FORMAT_U8,                           /**< PMC format setting  */
    HAL_AUDIO_PCM_FORMAT_S16_LE,                       /**< PMC format setting  */
    HAL_AUDIO_PCM_FORMAT_S16_BE,                       /**< PMC format setting  */
    HAL_AUDIO_PCM_FORMAT_U16_LE,                       /**< PMC format setting  */
    HAL_AUDIO_PCM_FORMAT_U16_BE,                       /**< PMC format setting  */
    HAL_AUDIO_PCM_FORMAT_S24_LE,                       /**< PMC format setting  */
    HAL_AUDIO_PCM_FORMAT_S24_BE,                       /**< PMC format setting  */
    HAL_AUDIO_PCM_FORMAT_U24_LE,                       /**< PMC format setting  */
    HAL_AUDIO_PCM_FORMAT_U24_BE,                       /**< PMC format setting  */
    HAL_AUDIO_PCM_FORMAT_S32_LE,                       /**< PMC format setting  */
    HAL_AUDIO_PCM_FORMAT_S32_BE,                       /**< PMC format setting  */
    HAL_AUDIO_PCM_FORMAT_U32_LE,                       /**< PMC format setting  */
    HAL_AUDIO_PCM_FORMAT_U32_BE,                       /**< PMC format setting  */
    HAL_AUDIO_PCM_FORMAT_LAST,                         /**< PMC format setting  */
    HAL_AUDIO_PCM_FORMAT_DUMMY  = 0xFFFFFFFF,               /**<  for DSP structrue alignment */
} hal_audio_pcm_format_t;

/** @brief Audio hal_audio_get_value_command_t. */
typedef enum {
    HAL_AUDIO_GET_AUDIO_STATUS                          =  0,       /**< get audio status  */
    HAL_AUDIO_GET_CONTROL_COUNT                         =  1,       /**< get control count  */
    HAL_AUDIO_GET_MEMORY_INPUT_CURRENT_OFFSET           =  2,       /**< get memory input current offset  */
    HAL_AUDIO_GET_MEMORY_OUTPUT_CURRENT_OFFSET          =  3,       /**< get memory output current offset  */
    HAL_AUDIO_GET_MEMORY_PLAYEN_MONITOR                 =  4,       /**< get memory play_en monitor */
    HAL_AUDIO_GET_MEMORY_SRC_INPUT_SAMPLE_COUNT         =  5,       /**< get memory src input sample count  */
    HAL_AUDIO_GET_MEMORY_INFORMATION                    =  6,       /**< get memory information  */

    HAL_AUDIO_GET_DEVICE_SAMPLE_RATE                    = 10,       /**< get device sample rate */
    HAL_AUDIO_GET_DEVICE_DAC_COMPENSATION_VALUE         = 11,       /**< get device DAC compensation value  */
    HAL_AUDIO_GET_SRC_XPPM                              = 12,       /**< get src xppm  */
    HAL_AUDIO_GET_CLOCK_SKEW_ASRC_COMPENSATED_SAMPLE    = 13,       /**< get clock skew asrc compensated sample  */

    HAL_AUDIO_GET_ANC_REG                               = 0x30,    /**<   get anc reg*/

} hal_audio_get_value_command_t;

/** @brief Audio hal_audio_set_value_command_t. */
typedef enum {
    HAL_AUDIO_SET_TRIGGER_MEMORY_START                  =  1,       /**<  SW trigger memory start */
    HAL_AUDIO_SET_SRC_INPUT_CURRENT_OFFSET              =  2,       /**<  SET SRC INPUT current OFFSET */
    HAL_AUDIO_SET_SRC_output_CURRENT_OFFSET             =  3,       /**<  SET SRC output current OFFSET */
    HAL_AUDIO_SET_IRQ_HANDLER                           =  4,       /**<  SET IRQ HANDLER */
    HAL_AUDIO_SET_SINE_GENERATOR                        =  5,       /**<   SET SINE GENERATOR*/
    HAL_AUDIO_SET_SRC_COMPENSATION                      =  6,       /**<   SET SRC COMPENSATION*/
    HAL_AUDIO_SET_MEMORY_IRQ_PERIOD                     =  7,       /**<   SET_MEMORY IRQ PERIOD */
    HAL_AUDIO_SET_MEMORY_IRQ_ENABLE                     =  8,       /**<   SET_MEMORY IRQ ENABLE */

    HAL_AUDIO_SET_DEVICE_AMP_OUTPUT_GPIO                =  0x10,    /**<   SET DEVICE AMP OUTPUT GPIO*/
    HAL_AUDIO_SET_DEVICE_OUTPUT_GPIO_DELAY_TIMER_MS     =  0x11,    /**<   SET DEVICE OUTPUT GPIO DELAY TIMER*/
    HAL_AUDIO_SET_DEVICE_AMP_DELAY_TIMER_MS             =  0x12,    /**<   SET DEVICE AMP DELAY TIMER*/
    HAL_AUDIO_SET_DEVICE_FORCE_OFF                      =  0x13,    /**<   SET DEVICE FORCE OFF*/
    HAL_AUDIO_SET_DEVICE_NOTICE_OFF_HANDLER             =  0x14,    /**<   SET DEVICE NOTICE OFF HANDLER*/
    HAL_AUDIO_SET_DEVICE_MIC_BIAS                       =  0x15,    /**<   SET DEVICE MIC BIAS*/
    HAL_AUDIO_SET_DEVICE_VAD_START                      =  0x16,    /**<   SET DEVICE VAD START*/
    HAL_AUDIO_SET_DEVICE_HOLD_AMP_OUTPUT_GPIO           =  0x17,    /**<   SET DEVICE HOLD AMP OUTPUT GPIO*/
    HAL_AUDIO_SET_DEVICE_SET_AMP_OUTPUT_GPIO_STATUS     =  0x18,    /**<   SET DEVICE SET AMP OUTPUT GPIO STATUS*/

    HAL_AUDIO_SET_VOLUME_HW_DIGITAL_SETTING             =  0x20,    /**<   SET VOLUME HW DIGITAL SETTING*/
    HAL_AUDIO_SET_VOLUME_HW_DIGITAL_GAIN                =  0x21,    /**<   SET VOLUME HW DIGITAL GAIN*/
    HAL_AUDIO_SET_VOLUME_HW_DIGITAL_FADE_TIME_SETTING   =  0x22,    /**<   SET VOLUME HW DIGITAL FADE TIME SETTING*/
    HAL_AUDIO_SET_VOLUME_INPUT_ANALOG_GAIN              =  0x23,    /**<   SET VOLUME INPUT ANALOG GAIN*/
    HAL_AUDIO_SET_VOLUME_OUTPUT_ANALOG_GAIN             =  0x24,    /**<   SET VOLUME OUTPUT ANALOG GAIN*/
    HAL_AUDIO_SET_VOLUME_OUTPUT_ANALOG_SETTING          =  0x25,    /**<   SET VOLUME OUTPUT ANALOG MODE*/
    HAL_AUDIO_SET_VOLUME_OUTPUT_ANALOG_GAIN_OFFSET      =  0x26,    /**<   SET VOLUME OUTPUT ANALOG GAIN OFFSET*/

    HAL_AUDIO_SET_ANC_REG                               =  0x30,    /**<   SET ANC REG*/

    HAL_AUDIO_SET_SLAVE_VDMA                            =  0x40,    /**<   SET SLAVE VDMA*/

    HAL_AUDIO_SET_AUDIO_CLOCK                           =  0x70,    /**<  SET AUDIO CLOCK */

} hal_audio_set_value_command_t;

/** @brief Audio hal_audio_irq_t. */
typedef enum {
    HAL_AUDIO_IRQ_AUDIOSYS                              =  1,  /**<   Set Audio irq*/
    HAL_AUDIO_IRQ_SRC1                                  =  2,  /**<   Set Audio irq*/
    HAL_AUDIO_IRQ_SRC2                                  =  3,  /**<   Set Audio irq*/
    HAL_AUDIO_IRQ_ANC                                   =  4,  /**<   Set Audio irq*/
    HAL_AUDIO_IRQ_VAD                                   =  5,  /**<   Set Audio irq*/
    HAL_AUDIO_IRQ_I2S_SLAVE                             =  6,  /**<   Set Audio irq*/
    HAL_AUDIO_VOW_SNR                                   =  7,  /**<   Set Audio irq*/
    HAL_AUDIO_VOW_FIFO                                  =  8,  /**<   Set Audio irq*/
} hal_audio_irq_t;

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

/** @brief Hal audio performance mode setting. */
typedef enum {
    AFE_PEROFRMANCE_NORMAL_MODE     = 0,           /**<  for mic/dac performance mode */
    AFE_PEROFRMANCE_HIGH_MODE       = 1,           /**<  for mic/dac performance mode */
    //The followings are only for UL
    AFE_PEROFRMANCE_LOW_POWER_MODE  = 2,          /**<  for mic performance mode */
    AFE_PEROFRMANCE_ULTRA_LOW_POWER_MODE  = 3,    /**<  for mic performance mode */
    AFE_PEROFRMANCE_SUPER_ULTRA_LOW_POWER_MODE  = 4, /**<  for mic performance mode */
    AFE_PEROFRMANCE__DUMMY     = 0x7FFFFFFF,   /**<  Dummy for DSP structure alignment */
} hal_audio_performance_mode_t;

/** @brief Hal audio dl sdm setting for loopback. */
typedef enum {
    AFE_AUDIO_DL_SDM_9_BIT          = 0,    /**< SDM*/
    AFE_AUDIO_DL_SDM_5_BIT          = 5,    /**< SDM 5 bit for UL1 loopback */
    AFE_AUDIO_DL_SDM_7_BIT          = 7,    /**< SDM 7 bit for UL4 loopback (ECO2 support) */
} hal_audio_dl_sdm_setting_t;

/** @brief Hal audio UL loopback setting. */
typedef enum {
    AFE_AUDIO_UL_LOOPBACK_NONE          = 0,    /**< Data from audio interface (Amic/Dmic) */
    AFE_AUDIO_UL_LOOPBACK_FROM_DL       = 1,    /**< Data from DL path (UL1 need to turn-on UL1 path) */
    AFE_AUDIO_UL_LOOPBACK_FROM_ANC      = 2,    /**< Data from ANC path(UL4 only) */
    AFE_AUDIO_UL_LOOPBACK_FROM_7BIT_SDM = 3,    /**< Data from DL 7bit SDM path(UL4 only after ECO2) */
} hal_audio_ul_loopback_setting_t;

/** @brief Hal audio src autio tracking mode setting. */
typedef enum  {
    HAL_AUDIO_SRC_TRACKING_DISABLE = 0, /**< ASRC tracking mode setting */
    HAL_AUDIO_SRC_TRACKING_I2S1,        /**< ASRC tracking mode setting */
    HAL_AUDIO_SRC_TRACKING_I2S2,        /**< ASRC tracking mode setting */
    HAL_AUDIO_SRC_TRACKING_I2S3,        /**< ASRC tracking mode setting */
    HAL_AUDIO_SRC_TRACKING_I2S4,        /**< ASRC tracking mode setting */
    HAL_AUDIO_SRC_TRACKING_SPDIFIN,     /**< ASRC tracking mode setting */
} hal_audio_src_tracking_clock_t;

/** @brief Hal audio src compensation control. */
typedef enum  {
    HAL_AUDIO_SRC_COMPENSATION_RESET = 0, /**< ASRC compensation setting */
    HAL_AUDIO_SRC_COMPENSATION_SET_VALUE, /**< ASRC compensation setting */
    HAL_AUDIO_SRC_COMPENSATION_INCREASE,  /**< ASRC compensation setting */
    HAL_AUDIO_SRC_COMPENSATION_DECREASE,  /**< ASRC compensation setting */
} hal_audio_src_compensation_control_t;

#ifdef ENABLE_HWSRC_CLKSKEW
/** @brief Hal audio src compensation control. */
typedef enum  {
    HAL_AUDIO_SRC_CLK_SKEW_V1 = 0, /**< sw clk skew */
    HAL_AUDIO_SRC_CLK_SKEW_V2,     /**< hwsrc clk skew, src always on */
} hal_audio_src_clk_skew_mode_t;
#endif

/** @brief Hal audio loopback test pga control. */
typedef enum  {
    HAL_AUDIO_LOOPBACK_TEST_PGA_NORMAL = 0, /**< pga mux in */
    HAL_AUDIO_LOOPBACK_TEST_PGA_NULL,       /**< pga mux null */
} hal_audio_loopback_test_pga_mode_t;

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

#ifdef AIR_AUDIO_I2S_SLAVE_TDM_ENABLE
/** @brief Hal audio I2S Slave TDM channel setting. */
typedef enum {
    HAL_AUDIO_I2S_TDM_DISABLE = 0x0,              /**< I2S TDM channel setting*/
    HAL_AUDIO_I2S_TDM_2CH     = 0x1,              /**< I2S TDM channel setting*/
    HAL_AUDIO_I2S_TDM_4CH     = 0x2,              /**< I2S TDM channel setting*/
    HAL_AUDIO_I2S_TDM_6CH     = 0x3,              /**< I2S TDM channel setting*/
    HAL_AUDIO_I2S_TDM_8CH     = 0x4,              /**< I2S TDM channel setting*/
} hal_audio_i2s_tdm_channel_setting_t;
#endif

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


/** @brief Hal audio dmic selection. */
typedef enum {
    HAL_AUDIO_DMIC_GPIO_DMIC0   = 0x0,                 /**<  for dmic selection */
    HAL_AUDIO_DMIC_GPIO_DMIC1,                         /**<  for dmic selection */
    HAL_AUDIO_DMIC_ANA_DMIC0,                          /**<  for dmic selection */
    HAL_AUDIO_DMIC_ANA_DMIC1,                          /**<  for dmic selection */
    HAL_AUDIO_DMIC_ANA_DMIC2,                          /**<  for dmic selection */
    HAL_AUDIO_DMIC_ANA_DMIC3,                          /**<  for dmic selection */
    HAL_AUDIO_DMIC_ANA_DMIC4,                          /**<  for dmic selection */
    HAL_AUDIO_DMIC_ANA_DMIC5,                          /**<  for dmic selection */
    HAL_AUDIO_DMIC_DUMMY        = 0xFFFFFFFF,           /**<  for DSP structrue alignment */
} hal_audio_dmic_selection_t;


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

/** @brief Hal audio vad input selection. */
typedef enum {
    HAL_AUDIO_VAD_INPUT_MIC0_P = 0,                    /**< vad input selection */
    HAL_AUDIO_VAD_INPUT_MIC0_N,                        /**< vad input selection */
    HAL_AUDIO_VAD_INPUT_MIC1_P,                        /**< vad input selection */
    HAL_AUDIO_VAD_INPUT_MIC1_N,                        /**< vad input selection */
} hal_audio_vad_input_select_t;
/** @brief Hal audio volume mute control. */
typedef enum {
    HAL_AUDIO_VOLUME_MUTE_FRAMEWORK     = 0,           /**<volume mute control setting */
    HAL_AUDIO_VOLUME_MUTE_ZERO_PADDING  = 1,           /**<volume mute control setting */
    HAL_AUDIO_VOLUME_MUTE_NUMBER        = 2,           /**<volume mute control setting */
} hal_audio_volume_mute_control_t;

/** @brief Hal audio vow mode setting. */
typedef enum {
    AFE_VOW_PHASE1 = 0,                               /**<vow mode setting */
    AFE_VOW_PHASE0,                                   /**<vow mode setting */
    AFE_VOW_PHASE0_PREROLL                            /**<vow mode setting */
}hal_audio_vow_mode_t;


/**
  * @}
  */

/** @defgroup hal_audio_struct Struct
  * @{
  */

/** @brief hal audio memory structure */
typedef struct {
    hal_audio_memory_selection_t memory_select; /**< for memory select */
    hal_audio_pcm_format_t pcm_format;          /**< for pcm format */
    hal_audio_memory_sync_selection_t sync_status; /**< for sync_status */

    uint32_t audio_path_rate;                       //Interconnection port rate /**< for audio_path_rate */
    uint32_t buffer_addr;                           /**< for buffer_addr */
    uint32_t buffer_length;                         /**< for buffer_length */

    uint32_t src_rate;                              //FW stream rate /**< for src_rate */
    uint32_t src_buffer_addr;                      /**< for src_buffer_addr */
    uint32_t src_buffer_length;                    /**< for src_buffer_length */

    uint16_t initial_buffer_offset;               /**< for initial_buffer_offset */
    uint16_t irq_counter; //Units:sample                     /**< for irq_counter */

    hal_audio_src_tracking_clock_t src_tracking_clock_source; /**< for src_tracking_clock_source */
    bool with_mono_channel;                      /**< for with_mono_channel */
    bool pure_agent_with_src;                    /**< for pure_agent_with_src */

#ifdef ENABLE_HWSRC_CLKSKEW
    hal_audio_src_clk_skew_mode_t asrc_clkskew_mode; /**< for asrc clk skew mode */
#endif
} hal_audio_memory_parameter_t;




/** @brief hal audio port parameter structure */
typedef union {
    hal_audio_device_interface_t device_interface; /**< for device_interface */
    hal_audio_memory_selection_t memory_select;   /**< for memory_select */
} hal_audio_path_port_parameter_t;

/** @brief hal audio port selection parameter structure */
typedef struct {
    hal_audio_interconn_sequence_t interconn_sequence[HAL_AUDIO_PATH_SUPPORT_SEQUENCE];/**< for interconn_sequence */
} hal_audio_path_selection_parameter_t;

/** @brief hal audio path parameter structure */
typedef struct {
    hal_audio_path_selection_parameter_t input; /**< for path_selection */
    hal_audio_path_selection_parameter_t output; /**< for path_selection */

    hal_audio_interconn_channel_selection_t connection_selection; /**< for interconn_channel_selection */
    uint32_t connection_number; /**< for connection_number */

    uint32_t audio_path_input_rate; /**< for audio_path_input_rate */
    uint32_t audio_path_output_rate; /**< for audio_path_output_rate */

    bool with_hw_gain; /**< for with_hw_gain */
    bool with_upwdown_sampler; /**< for with_upwdown_sampler */
    bool with_dl_deq_mixer; /**< for with_dl_deq_mixer */
} hal_audio_path_parameter_t;


/** @brief hal audio device done closure entry for delay off procedure */
typedef void (*hal_audio_closure_entry)(void);

/** @brief  hal audio device parameter adc*/
typedef struct {
    uint8_t adc_mode;       //hal_audio_analog_mdoe_t /**< for adc_mode */
    uint8_t performance;    //hal_audio_performance_mode_t /**< for performance */
    bool    is_fifo_sync_clock_ch1; /**< for is_fifo_sync_clock_ch1 */
    bool    with_au_in_swap; /**< for with_au_in_swap */
} hal_audio_device_parameter_adc_t;

/** @brief  hal audio device parameter analog mic*/
typedef struct {
    hal_audio_device_t audio_device; /**< for audio_device */
    hal_audio_device_interface_t mic_interface;/**< for mic_interface */
    uint32_t rate;  /**< for rate */

    hal_audio_bias_voltage_t bias_voltage[5]; /**< for bias_voltage */
    hal_audio_bias_selection_t bias_select;/**< for bias_select */
    hal_audio_ul_iir_t iir_filter; /**< for iir_filter */

    hal_audio_dmic_selection_t _reseved0;/**< for _reseved0 */
    hal_audio_device_parameter_adc_t adc_parameter;/**< for adc_parameter */
    uint8_t _reseved1;/**for afe_dmic_clock_rate_t*/
    bool with_external_bias;/**< for with_external_bias */
    bool with_bias_lowpower;/**< for with_bias_lowpower */
    bool bias1_2_with_LDO0;/**< for bias1_2_with_LDO0 */

    bool with_phase_inverse;/**< for with_phase_inverse */
} hal_audio_device_parameter_analog_mic_t;

/** @brief  hal audio device parameter digital mic*/
typedef struct {
    hal_audio_device_t audio_device;/**< for audio_device */
    hal_audio_device_interface_t mic_interface;/**< for mic_interface */
    uint32_t rate; /**< for rate */

    hal_audio_bias_voltage_t bias_voltage[5]; /**< for bias_voltage */
    hal_audio_bias_selection_t bias_select;/**< for bias_select */
    hal_audio_ul_iir_t iir_filter;/**< iir_filter */

    hal_audio_dmic_selection_t dmic_selection;/**< for dmic_selection */
    hal_audio_device_parameter_adc_t adc_parameter;/**< for adc_parameter */
    uint8_t dmic_clock_rate;/**for afe_dmic_clock_rate_t*/
    bool with_external_bias;/**< for with_external_bias */
    bool with_bias_lowpower;/**< for with_bias_lowpower */
    bool bias1_2_with_LDO0;/**< for bias1_2_with_LDO0 */
} hal_audio_device_parameter_digital_mic_t;

/** @brief  hal audio device parameter loopback*/
typedef struct {
    hal_audio_device_t audio_device;/**< for audio_device */
    hal_audio_device_interface_t ul_interface;/**< for ul_interface */
    uint32_t rate;/**< for rate */

    hal_audio_ul_loopback_setting_t loopback_setting;/**< for loopback_setting */
    hal_audio_ul_iir_t iir_filter;/**< for iir_filter */
} hal_audio_device_parameter_loopback_t;

/** @brief  hal audio device parameter linein*/
typedef struct {
    hal_audio_device_t audio_device;/**< for audio_device */
    hal_audio_device_interface_t reserved;/**< for reserved */
    uint32_t rate;/**< for rate */

    hal_audio_bias_voltage_t bias_voltage[5];/**< for bias_voltage */
    hal_audio_bias_selection_t bias_select;/**< for bias_select */
    hal_audio_ul_iir_t iir_filter;/**< for iir_filter */
    hal_audio_device_parameter_adc_t adc_parameter;/**< for adc_parameter */

    bool with_phase_inverse;/**< for with_phase_inverse */
} hal_audio_device_parameter_linein_t;

/** @brief  hal audio device parameter dac*/
typedef struct {
    hal_audio_device_t audio_device;/**< for audio_device */
    hal_audio_device_interface_t reserved;/**< for reserved */
    uint32_t rate;/**< for rate */

    hal_audio_analog_mdoe_t dac_mode;/**< for dac_mode */
    uint32_t dc_compensation_value;/**< for dc_compensation_value */

    //hal_audio_dl_sdm_setting_t, For loopback setting
    uint8_t dl_sdm_setting;       /**< for dl_sdm_setting */
    bool with_high_performance;   /**< for with_high_performance */
    bool with_phase_inverse;      /**< for with_phase_inverse */

    //For ANC stay alone
    bool with_force_change_rate; /**< for with_force_change_rate */
} hal_audio_device_parameter_dac_t;

/** @brief  hal audio device parameter i2s master*/
typedef struct {
    hal_audio_device_t audio_device;/**< for audio_device */
    hal_audio_device_interface_t i2s_interface;/**< for i2s_interface */
    uint32_t rate;/**< for rate */

    hal_audio_i2s_format_t i2s_format;/**< for i2s_format */
    hal_audio_i2s_word_length_t word_length;/**< for word_length */
    uint32_t mclk_divider;/**< for mclk_divider */
    bool with_mclk;/**< for with_mclk */
    bool is_low_jitter;/**< for is_low_jitter */
    bool is_rx_swap;/**< for is_rx_swap */
    bool is_tx_swap;/**< for is_tx_swap */
    bool is_internal_loopback;/**< for is_internal_loopback */
    //TDM for G-sensor
    bool is_recombinant;/**< for is_recombinant */
} hal_audio_device_parameter_i2s_master_t;

/** @brief  hal audio device parameter i2s slave*/
typedef struct {
    hal_audio_device_t audio_device;/**< for audio_device */
    hal_audio_device_interface_t i2s_interface;/**< for i2s_interface */
    uint32_t rate;/**< for rate */

    hal_audio_i2s_format_t i2s_format;/**< for i2s_format */
    hal_audio_i2s_word_length_t word_length;/**< for word_length */
    hal_audio_memory_selection_t memory_select;/**< for memory select */
#ifdef AIR_AUDIO_I2S_SLAVE_TDM_ENABLE
    hal_audio_i2s_tdm_channel_setting_t tdm_channel;/**< for tdm channel select */
#endif
    bool is_rx_swap;/**< for is_rx_swap */
    bool is_tx_swap;/**< for is_tx_swap */
    bool is_vdma_mode;/**< for is_vdma_mode */
} hal_audio_device_parameter_i2s_slave_t;

/** @brief  hal audio device parameter spdif */
typedef struct {
    hal_audio_device_parameter_i2s_master_t i2s_setting;/**< for i2s_setting */
} hal_audio_device_parameter_spdif_t;

/** @brief  hal audio device parameter sidetone */
typedef struct {
    hal_audio_device_t audio_device;/**< for audio_device */
    hal_audio_device_interface_t reserved;/**< for reserved */
    uint32_t rate;/**< for rate */

    hal_audio_closure_entry sidetone_stop_done_entry;/**< for sidetone_stop_done_entry */
    hal_audio_control_t input_device;/**< for input_device */
    hal_audio_device_interface_t input_interface;/**< for input_interface */
    hal_audio_control_t output_device;/**< for output_device */
    hal_audio_device_interface_t output_interface;/**< for output_interface */

    hal_audio_interconn_selection_t input_interconn_select;/**< for input_interconn_select */

    int32_t sidetone_gain;/**< for sidetone_gain */
    uint16_t *p_sidetone_FIR_coef;/**<for p_sidetone_FIR_coef >*/
    bool is_sidetone_gain_register_value;/**< for sidetone_gain_register_value */
    bool with_gain_ramp;/**< for gain_ramp */
} hal_audio_device_parameter_sidetone_t;

/** @brief  hal audio device parameter vad*/
typedef struct {
    hal_audio_device_t audio_device;/**< for audio_device */
    hal_audio_device_interface_t reserved;/**< for reserved */
    uint32_t rate;/**< for rate */

    hal_audio_bias_voltage_t bias_voltage[5];/**< for bias_voltage */
    hal_audio_bias_selection_t bias_select;/**< for bias_select */

    U16             threshold_0;/**< for threshold_0 */
    U16             threshold_1;/**< for threshold_1 */

    //hal_audio_vad_input_select_t
    U8              input_sel;/**< for input_sel */
    U8              amp_gain;/**< for amp_gain */
    bool            with_bias_lowpower;/**< for with_bias_lowpower */

} hal_audio_device_parameter_vad_t;


/** @brief  hal audio device parameter vow*/
typedef struct {
    hal_audio_device_t audio_device;/**< for audio_device */
    hal_audio_device_interface_t mic_interface;/**< for mic_interface */
    uint32_t rate;/**< for rate */

    hal_audio_bias_voltage_t bias_voltage[5];/**< for bias_voltage */
    hal_audio_bias_selection_t bias_select;/**< for bias_select */
    hal_audio_closure_entry vow_detection_done_entry;/**< for vow_detection_done_entry */
    hal_audio_control_t input_device;/**< for input_device */
    hal_audio_control_t mic_selection;/**< for mic_selection */
    hal_audio_memory_selection_t memory_select;/**< for memory_select */
    hal_audio_dmic_selection_t dmic_selection;/**< for dmic_selection */
    hal_audio_device_parameter_adc_t adc_parameter;/**< for adc_parameter */

    hal_audio_vow_mode_t vow_mode;/**< for vow_mode */

    uint32_t snr_threshold;/**< set snr threshold, 0xX3X3,X = 0~7, Least sensitive:0x7373, Middle sensitive:0x4343,Very sensitive:0x0303*/
    uint32_t noise_ignore_bit;/**< ignore vow's the number of bit of noise signal*/
    uint8_t dma_irq_threshold;/**< Units:sample */
    uint8_t alpha_rise;/**< set k_alpha_rise for signal frame size, range = 0x0~0xF, the smaller the faster, the defaut value is 0x7*/
    bool suspend_mic;/**< switch DMIC to low power mode, AMIC to ultra low power mode*/
    bool vow_with_hpf;/**< for vow_with_hpf */
    bool with_external_bias;/**< for with_external_bias */
    bool with_bias_lowpower;/**< for with_bias_lowpower */
    bool bias1_2_with_LDO0;/**< for bias1_2_with_LDO0 */
} hal_audio_device_parameter_vow_t;

/** @brief  hal audio device parameter common */
typedef struct {
    hal_audio_device_t audio_device;/**< for audio_device */
    hal_audio_device_interface_t device_interface;/**< for device_interface */
    uint32_t rate;/**< for rate */
} hal_audio_device_parameter_common_t;

/** @brief hal audio device parameter structure */
typedef union {
    hal_audio_device_parameter_analog_mic_t     analog_mic;/**< for analog_mic */
    hal_audio_device_parameter_linein_t         linein;/**< for linein */
    hal_audio_device_parameter_digital_mic_t    digital_mic;/**< for digital_mic */
    hal_audio_device_parameter_loopback_t       loopback;/**< for loopback */
    hal_audio_device_parameter_dac_t            dac;/**< for dac */
    hal_audio_device_parameter_i2s_master_t     i2s_master;/**< for i2s_master */
    hal_audio_device_parameter_i2s_slave_t      i2s_slave;/**< for i2s_slave */
    hal_audio_device_parameter_spdif_t          spdif;/**< for spdif */
    hal_audio_device_parameter_sidetone_t       sidetone;/**< for sidetone */
    hal_audio_device_parameter_vad_t            vad;/**< for vad */
    hal_audio_device_parameter_vow_t            vow;/**< for vow */
    hal_audio_device_parameter_common_t         common;/**< for common */
} hal_audio_device_parameter_t;




/** @brief hal audio IRQ entry structure */
typedef void (*hal_audio_irq_entry)(void);

/** @brief hal audio irq parameter structure */
typedef struct {
    hal_audio_irq_t audio_irq;
    hal_audio_memory_selection_t memory_select;
    hal_audio_irq_entry entry;
} hal_audio_irq_parameter_t;

/** @brief hal audio current offset parameter structure */
typedef struct {
    hal_audio_memory_selection_t memory_select;
    uint32_t    offset;
    uint32_t    base_address;
    bool        pure_agent_with_src;
} hal_audio_current_offset_parameter_t;

/** @brief hal audio trigger start parameter structure */
typedef struct {
    hal_audio_memory_selection_t memory_select;
    bool enable;
} hal_audio_trigger_start_parameter_t;

/** @brief hal audio trigger start parameter structure */
typedef struct {
    hal_audio_memory_selection_t memory_select;
    uint32_t buffer_addr;
    uint32_t buffer_length;
    uint32_t buffer_current_offset;
    bool is_enable;
} hal_audio_memory_information_parameter_t;

/** @brief hal audio sine generator parameter structure */
typedef struct {
    hal_audio_control_t audio_control;
    hal_audio_path_port_parameter_t port_parameter;
    uint32_t rate;
    bool is_input_port;
    bool enable;
} hal_audio_sine_generator_parameter_t;

/** @brief hal audio src compensation parameter structure */
typedef struct {
    hal_audio_memory_selection_t memory_select;
    hal_audio_src_compensation_control_t control;
    int32_t compensation_value;
} hal_audio_src_compensation_parameter_t;

/** @brief hal audio src clock skew compensation parameter structure */
typedef struct {
    int32_t* accumulate_array;
} hal_audio_src_clock_skew_compensation_parameter_t;


/** @brief hal audio memory irq period parameter structure */
typedef struct {
    hal_audio_memory_selection_t memory_select;
    uint32_t  rate;
    uint32_t irq_counter;
} hal_audio_memory_irq_period_parameter_t;

/** @brief hal audio memory irq enable parameter structure */
typedef struct {
    hal_audio_memory_selection_t memory_select;
    uint32_t  rate;
    uint32_t irq_counter;
    bool enable;
} hal_audio_memory_irq_enable_parameter_t;

/** @brief hal audio volume digital gain setting parameter structure */
typedef struct {
    hal_audio_memory_selection_t memory_select;
    uint32_t index_compensation;
    uint32_t sample_per_step;
} hal_audio_volume_digital_gain_setting_parameter_t;

/** @brief hal audio volume digital gain fade time setting  structure */
typedef struct {
    hal_audio_memory_selection_t memory_select;
    uint32_t fade_time;
    uint32_t gain_index;
} hal_audio_volume_digital_gain_fade_time_setting_parameter_t;

/** @brief hal audio volume digital gain parameter structure */
typedef struct {
    hal_audio_memory_selection_t memory_select;
    uint32_t    value;
    hal_audio_volume_mute_control_t mute_control;

    bool mute_enable;
    bool is_mute_control;
    bool is_set_by_register;
} hal_audio_volume_digital_gain_parameter_t;

/** @brief hal audio volume analog input gain parameter structure */
typedef struct {
    hal_audio_device_interface_t device_interface;
    uint32_t    value_l;
    uint32_t    value_r;
    bool is_set_by_register;
} hal_audio_volume_analog_input_gain_parameter_t;

/** @brief hal audio volume analog output gain parameter structure */
typedef struct {
    uint32_t    value_l;
    uint32_t    value_r;
    bool is_set_by_register;
} hal_audio_volume_analog_output_gain_parameter_t;

/** @brief hal audio volume analog output mode parameter structure */
typedef struct {
    uint32_t gain_select;/**< define afe_hardware_analog_gain_t */
    hal_audio_analog_mdoe_t dac_mode;/**< for dac_mode */
} hal_audio_volume_analog_output_mode_parameter_t;


/** @brief hal audio device select parameter structure */
typedef struct {
    hal_audio_control_t device_control;
    hal_audio_device_interface_t device_interface;
} hal_audio_device_select_parameter_t;

/** @brief hal audio control select parameter structure */
typedef struct {
    hal_audio_control_t audio_control;
    hal_audio_path_port_parameter_t audio_port;
} hal_audio_control_select_parameter_t;

/** @brief hal audio vad start parameter structure */
typedef struct {
    bool enable;
} hal_audio_vad_start_parameter_t;

/** @brief hal audio mic bias parameter structure */
typedef struct {
    hal_audio_bias_voltage_t bias_voltage[5];
    hal_audio_bias_selection_t bias_select;

    bool with_bias_lowpower;
    bool bias1_2_with_LDO0;
    bool enable;
} hal_audio_mic_bias_parameter_t;

/** @brief hal audio slave vdma parameter structure */
typedef struct {
    uint32_t base_address;
    uint32_t size;
    uint32_t threshold;
    hal_audio_interface_t audio_interface;
#ifdef AIR_AUDIO_I2S_SLAVE_TDM_ENABLE
    hal_audio_i2s_tdm_channel_setting_t tdm_channel;
    bool enable;
#endif
    bool is_ul_mode;
} hal_audio_slave_vdma_parameter_t;

/** @brief hal audio get audio related value parameter structure */
typedef union {
    uint32_t                                            value;                              /**< for COMMON */
    hal_audio_control_select_parameter_t                get_control_count;                  /**< for HAL_AUDIO_GET_CONTROL_COUNT */
    hal_audio_current_offset_parameter_t                get_current_offset;                 /**< for HAL_AUDIO_GET_MEMORY_INPUT_CURRENT_OFFSET and HAL_AUDIO_GET_MEMORY_OUTPUT_CURRENT_OFFSET */
    hal_audio_device_select_parameter_t                 get_device_rate;                    /**< for HAL_AUDIO_GET_DEVICE_SAMPLE_RATE */
    hal_audio_src_compensation_parameter_t              src_compensation;                   /**< for HAL_AUDIO_GET_SRC_XPPM */
    hal_audio_memory_selection_t                        get_src_sample_count;               /**< for HAL_AUDIO_GET_MEMORY_SRC_INPUT_SAMPLE_COUNT */
    hal_audio_memory_information_parameter_t            get_memory_information;             /**< for HAL_AUDIO_GET_MEMORY_INFORMATION */
    hal_audio_analog_mdoe_t                             get_dl_dc_compensation;             /**< for HAL_AUDIO_GET_DEVICE_DAC_COMPENSATION_VALUE */
    hal_audio_src_clock_skew_compensation_parameter_t   get_src_clock_skew_cp;              /**< for HAL_AUDIO_GET_SRC_CLOCK_SKEW_COMPENSATION_VALUE */
} hal_audio_get_value_parameter_t;


/** @brief hal audio set audio related value parameter structure */
typedef union {
    uint32_t                                            value;                  /**< for COMMON */
    hal_audio_trigger_start_parameter_t                 sw_trigger_start;       /**< for HAL_AUDIO_SET_TRIGGER_MEMORY_START */
    hal_audio_current_offset_parameter_t                set_current_offset;     /**< for HAL_AUDIO_SET_SRC_INPUT_CURRENT_OFFSET and HAL_AUDIO_SET_SRC_output_CURRENT_OFFSET */
    hal_audio_irq_parameter_t                           register_irq_handler;   /**< for HAL_AUDIO_SET_IRQ_HANDLER */
    hal_audio_sine_generator_parameter_t                sine_generator;         /**< for HAL_AUDIO_SET_SINE_GENERATOR */
    hal_audio_src_compensation_parameter_t              src_compensation;       /**< for HAL_AUDIO_SET_SRC_COMPENSATION */
    hal_audio_memory_irq_period_parameter_t             irq_period;             /**< for HAL_AUDIO_SET_MEMORY_IRQ_PERIOD */
    hal_audio_memory_irq_enable_parameter_t             irq_enable;             /**< for HAL_AUDIO_SET_MEMORY_IRQ_ENABLE */
    hal_audio_volume_digital_gain_setting_parameter_t   digital_gain_setting;   /**< for HAL_AUDIO_SET_VOLUME_HW_DIGITAL_SETTING */
    hal_audio_volume_digital_gain_fade_time_setting_parameter_t digital_gain_fade_time_setting;   /**< for HAL_AUDIO_SET_VOLUME_HW_DIGITAL_FADE_TIME_SETTING */
    hal_audio_volume_digital_gain_parameter_t           digital_gain;           /**< for HAL_AUDIO_SET_VOLUME_HW_DIGITAL_GAIN */
    hal_audio_volume_analog_input_gain_parameter_t      analog_input_gain;      /**< for HAL_AUDIO_SET_VOLUME_INPUT_ANALOG_GAIN */
    hal_audio_volume_analog_output_gain_parameter_t     analog_output_gain;     /**< for HAL_AUDIO_SET_VOLUME_OUTPUT_ANALOG_GAIN */
    hal_audio_volume_analog_output_mode_parameter_t     analog_output_mode;
    hal_audio_mic_bias_parameter_t                      mic_bias;               /**< for HAL_AUDIO_SET_DEVICE_MIC_BIAS */
    hal_audio_vad_start_parameter_t                     vad_start;              /**< for HAL_AUDIO_SET_DEVICE_VAD_START */
    hal_audio_slave_vdma_parameter_t                    slave_vdma;             /**< for HAL_AUDIO_SET_SLAVE_VDMA */
} hal_audio_set_value_parameter_t;



/**
  * @}
  */


/**
  * @ Initialize hal audio
  */
void hal_audio_initialize(void);


/**
  * @ Set audio device
  * @ handle :
  * @ device :
  * @ control :
  * @ Retval: HAL_AUDIO_STATUS_OK if operation is successful, others if channel number is invalid
  */
hal_audio_status_t hal_audio_set_device(hal_audio_device_parameter_t *handle, hal_audio_control_t device, hal_audio_control_status_t control);
/**
  * @ Set audio path
  * @ handle :
  * @ device :
  * @ control :
  * @ Retval: HAL_AUDIO_STATUS_OK if operation is successful, others if channel number is invalid
  */
hal_audio_status_t hal_audio_set_memory(hal_audio_memory_parameter_t *handle, hal_audio_control_t memory_interface, hal_audio_control_status_t control);

/**
  * @ Set audio path
  * @ handle :
  * @ device :
  * @ control :
  * @ Retval: HAL_AUDIO_STATUS_OK if operation is successful, others if channel number is invalid
  */
hal_audio_status_t hal_audio_set_path(hal_audio_path_parameter_t *handle, hal_audio_control_status_t control);

/**
  * @ Get audio related status
  * @ handle :
  * @ command :
  * @ Retval: value
  */
uint32_t hal_audio_get_value(hal_audio_get_value_parameter_t *handle, hal_audio_get_value_command_t command);

/**
  * @ Set audio related status
  * @ handle :
  * @ device :
  * @ control :
  * @ Retval: HAL_AUDIO_STATUS_OK if operation is successful, others if channel number is invalid
  */
hal_audio_status_t hal_audio_set_value(hal_audio_set_value_parameter_t *handle, hal_audio_set_value_command_t command);




#define HAL_AUDIO_INVALID_ANALOG_GAIN_INDEX (0x7FFF)
#define HAL_AUDIO_INVALID_DIGITAL_GAIN_INDEX (0x7FFF)




//#endif

/** @brief audio channel number define */
typedef enum {
    HAL_AUDIO_MONO                  = 0, /**< A single channel.  */
    HAL_AUDIO_STEREO                = 1, /**< Two channels. */
    HAL_AUDIO_STEREO_BOTH_L_CHANNEL = 2, /**< Two channels, but only output L channel. That is (L, R) -> (L, L). */
    HAL_AUDIO_STEREO_BOTH_R_CHANNEL = 3, /**< Two channels, but only output R channel. That is (L, R) -> (R, R). */
    HAL_AUDIO_STEREO_BOTH_L_R_SWAP  = 4  /**< Two channels, L and R channels are swapped. That is (L, R) -> (R, L). */
} hal_audio_channel_number_t;

/** @brief audio stream out define */
typedef enum {
    HAL_AUDIO_STREAM_OUT1           = 0, /**<  stream out HWGAIN1 only. */
    HAL_AUDIO_STREAM_OUT2           = 1, /**<  stream out HWGAIN2 only. */
    HAL_AUDIO_STREAM_OUT_ALL        = 2, /**<  stream out HWGAIN1 & HWGAIN2 & HWGAIN3. */
    HAL_AUDIO_STREAM_OUT3           = 3, /**<  stream out HWGAIN3 only. */
} hal_audio_hw_stream_out_index_t;

/**
  * @}
  */

/** @defgroup hal_audio_struct Struct
  * @{
  */

/** @brief Audio callback handler structure */
typedef struct {
    void  (*afe_dl1_interrupt_handler)(void);
    void  (*afe_dl2_interrupt_handler)(void);
    void  (*afe_dl3_interrupt_handler)(void);
    void  (*afe_vul1_interrupt_handler)(void);
    void  (*afe_vul2_interrupt_handler)(void);
    void  (*afe_awb_interrupt_handler)(void);
    void  (*afe_awb2_interrupt_handler)(void);
    void  (*afe_dl12_interrupt_handler)(void);
    void  (*afe_anc_pwrdet_interrupt_handler)(uint32_t irq_index);
} hal_audio_irq_callback_function_t;

/** @brief Audio Amp function structure */
typedef struct {
    bool (*open_handler)(uint32_t samplerate);
    bool (*closure_handler)(void);

    bool (*change_dac_rate_handler)(uint32_t samplerate);
    bool (*get_dac_rate_change_status)(uint32_t *samplerate);
    bool (*set_hw_gain_id_handler)(uint8_t hw_gain_id);
} hal_amp_function_t;





/**
  * @}
  */

/**
 * @brief     Initializes basic settings of the audio hardware
 * @note      This function should at least be called once after system restarts and before using any audio functionality.
 */
void hal_audio_init(void);

/**
 * @brief     Set the audio output device
 * @param[in] device is the ouput component. For more details, please refer to #hal_audio_device_t.
 * @return    #HAL_AUDIO_STATUS_OK, if output device is valid.
 * @par       Example
 * @code      hal_audio_set_stream_out_device(HAL_AUDIO_DEVICE_HEADSET);
 * @endcode
 */
hal_audio_status_t hal_audio_set_stream_out_device(hal_audio_device_t device);

/**
 * @brief     Set audio input device
 * @param[in] device is the input component. For more details, please refer to #hal_audio_device_t.
 * @return    #HAL_AUDIO_STATUS_OK, if input device is valid.
 * @par       Example
 * @code      hal_audio_set_stream_in_device(HAL_AUDIO_DEVICE_MAIN_MIC);
 * @endcode
 */
hal_audio_status_t hal_audio_set_stream_in_device(hal_audio_device_t device);

/**
 * @brief     Get current audio output device
 */
hal_audio_device_t hal_audio_get_stream_out_device(void);

/**
 * @brief     Get current audio input device
 */
hal_audio_device_t hal_audio_get_stream_in_device(void);

/**
 * @brief     Set audio output the volume.
 * @param[in] digital_volume_index is to set digital gain in centi-db in hex (FE0C equal to -5db). Gain range: depends on the gain map.
 * @param[in] analog_volume_index is to set digital gain in centi-db in hex (FE0C equal to -5db). Gain range: depends on the hardware design and gain map.
 */
void hal_audio_set_stream_out_volume(uint16_t digital_volume_index, uint16_t analog_volume_index);

/**
 * @brief     Set audio output the volume for dl2
 * @param[in] digital_volume_index is to set digital gain in centi-db in hex (FE0C equal to -5db). Gain range: depends on the gain map.
 * @param[in] analog_volume_index is to set digital gain in centi-db in hex (FE0C equal to -5db). Gain range: depends on the hardware design and gain map.
 */
void hal_audio_set_stream_out_volume2(uint16_t digital_volume_index, uint16_t analog_volume_index);

/**
 * @brief     Set audio output the volume for dl3.
 * @param[in] digital_volume_index is to set digital gain in centi-db in hex (FE0C equal to -5db). Gain range: depends on the gain map.
 * @param[in] analog_volume_index is to set digital gain in centi-db in hex (FE0C equal to -5db). Gain range: depends on the hardware design and gain map.
 */
void hal_audio_set_stream_out_volume3(uint16_t digital_volume_index, uint16_t analog_volume_index);

#ifdef AIR_AUDIO_LR_OUT_ANALOG_GAIN_OFFSET_ENABLE
/**
 * @brief     Set audio output offset volume.
 * @param[in] analog_volume_l_offset_index is to set digital gain in centi-db in hex (FE0C equal to -5db). Gain range: -4db to 4db.
 * @param[in] analog_volume_r_offset_index is to set digital gain in centi-db in hex (FE0C equal to -5db). Gain range: -4db to 4db.
 */
void hal_audio_set_stream_out_offset_volume(uint16_t analog_volume_l_offset_index, uint16_t analog_volume_r_offset_index);
#endif

#if defined(HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT)
/**
 * @brief     Mute the audio output stream
 * @param[in] mute is a flag to set the audio on/off silent.
 * @param[in] hw_gain_index is a flag to indicate specific stream out device.
 * @par       example
 * @code      hal_audio_mute_stream_out(true, HAL_AUDIO_STREAM_OUT1);
 * @endcode
 */
void hal_audio_mute_stream_out(bool mute, hal_audio_hw_stream_out_index_t hw_gain_index);
#else
/**
 * @brief     Mute the audio output stream
 * @param[in] mute is a flag to set the audio on/off silent.
 * @par       example
 * @code      hal_audio_mute_stream_out(true);
 * @endcode
 */
void hal_audio_mute_stream_out(bool mute);
#endif

/**
 * @brief     Set audio input volume for multiple microphones.
 * @param[in] volume_index0 is to set a input gain in centi-db in hex (FE0C equal to -5db). Gain range: depends on the hardware design and gain map.
 * @param[in] volume_index1 is to set a input gain in centi-db in hex (FE0C equal to -5db). Gain range: depends on the hardware design and gain map.
 * @param[in] gain_select is to select which pair of gain to be setting.
 */
#ifndef HAL_AUDIO_ENABLE_PATH_MEM_DEVICE
void hal_audio_set_stream_in_volume_for_multiple_microphone(uint16_t volume_index0, uint16_t volume_index1, hal_audio_input_gain_select_t gain_select);
#endif
/**
 * @brief     Set audio input volume
 * @param[in] digital_volume_index is to set a digital gain in centi-db in hex (FE0C equal to -5db). Gain range: depend on the gain map.
 * @param[in] analog_volume_index is to set digital gain in centi-db in hex (FE0C equal to -5db). Gain range: depends on the hardware design and gain map.
 */
void hal_audio_set_stream_in_volume(uint16_t digital_volume_index, uint16_t analog_volume_index);

/**
 * @brief     Mute the audio input stream
 * @param[in] mute is a flag to set the audio on/off silent.
 * @par       example
 * @code      hal_audio_mute_stream_in(true);
 * @endcode
 */
void hal_audio_mute_stream_in(bool mute);

/**
 * @brief     Hook audio front end memory interface irq handler
 * @param[in] function is a structure pointer to audio irq callback function.
 */
void hal_audio_afe_register_irq_callback(hal_audio_irq_callback_function_t* function);

/**
 * @brief     Set the audio output channel number.
 * @param[in] channel_number is to set the output channel number. For more details, please refer to #hal_audio_channel_number_t.
 * @return    #HAL_AUDIO_STATUS_OK, if channel number is valid.
 */
hal_audio_status_t hal_audio_set_stream_out_channel_number(hal_audio_channel_number_t channel_number);

/**
 * @brief     Set audio input channel number
 * @param[in] channel_number is to set input channel number. For more details, please refer to #hal_audio_channel_number_t.
 * @return    #HAL_AUDIO_STATUS_OK, if channel number is valid.
 */
hal_audio_status_t hal_audio_set_stream_in_channel_number(hal_audio_channel_number_t channel_number);

/**
 * @brief     Get the audio output channel number.
 * @return    channel_number for output.
 */
hal_audio_channel_number_t hal_audio_get_stream_out_channel_number(void);


/**
 * @brief     Hook audio amp handle
 * @param[in] function is structure pointer to audio amp function.
 */
void hal_audio_afe_register_amp_handle(hal_amp_function_t *function);

void hal_audio_afe_set_play_en(U32 nat_clk, U32 intra_clk);

#ifdef __cplusplus
}
#endif

#endif /*HAL_AUDIO_MODULE_ENABLED*/
#endif /* __HAL_AUDIO_H__ */
