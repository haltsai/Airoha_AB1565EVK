/* Copyright Statement:
 *
 * (C) 2018  Airoha Technology Corp. All rights reserved.
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

#ifndef __HAL_AUDIO_PATH_H__
#define __HAL_AUDIO_PATH_H__

#include "hal_audio.h"

#ifdef HAL_AUDIO_MODULE_ENABLED

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Type Definitions ///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*Align AFE RG bit operation*/
typedef enum {
    AUDIO_INTERCONNECTION_INPUT_I00 = 0,        /**<  I2S0 master input CH1 */
    AUDIO_INTERCONNECTION_INPUT_I01,            /**<  I2S0 master input CH2 */
    AUDIO_INTERCONNECTION_INPUT_I02,            /**<  I2S1 master input CH1 */
    AUDIO_INTERCONNECTION_INPUT_I03,            /**<  I2S1 master input CH2 */
    AUDIO_INTERCONNECTION_INPUT_I04,            /**<  I2S2 master input CH1 */
    AUDIO_INTERCONNECTION_INPUT_I05,            /**<  I2S2 master input CH2 */
    AUDIO_INTERCONNECTION_INPUT_I06,            /**<  HW Gain3 output CH1 */
    AUDIO_INTERCONNECTION_INPUT_I07,            /**<  HW Gain3 output CH2 */
    AUDIO_INTERCONNECTION_INPUT_I08,            /**<  UL1 input CH1 */
    AUDIO_INTERCONNECTION_INPUT_I09,            /**<  UL1 input CH2 */
    AUDIO_INTERCONNECTION_INPUT_I10,            /**<  UL2 input CH1 */
    AUDIO_INTERCONNECTION_INPUT_I11,            /**<  UL2 input CH2 */
    AUDIO_INTERCONNECTION_INPUT_I12,            /**<  UL3 input CH1 */
    AUDIO_INTERCONNECTION_INPUT_I13,            /**<  UL3 input CH2 */
    AUDIO_INTERCONNECTION_INPUT_I14,            /**<  HW Gain1 output CH1 */
    AUDIO_INTERCONNECTION_INPUT_I15,            /**<  HW Gain1 output CH2 */
    AUDIO_INTERCONNECTION_INPUT_I16,            /**<  HW Gain2 output CH1 */
    AUDIO_INTERCONNECTION_INPUT_I17,            /**<  HW Gain2 output CH2 */
    AUDIO_INTERCONNECTION_INPUT_I18,            /**<  DL1 data output CH1 */
    AUDIO_INTERCONNECTION_INPUT_I19,            /**<  DL1 data output CH2 */
    AUDIO_INTERCONNECTION_INPUT_I20,            /**<  DL2 data output CH1 */
    AUDIO_INTERCONNECTION_INPUT_I21,            /**<  DL2 data output CH2 */
    AUDIO_INTERCONNECTION_INPUT_I22,            /**<  DL3 data output CH1 */
    AUDIO_INTERCONNECTION_INPUT_I23,            /**<  DL3 data output CH2 */
    AUDIO_INTERCONNECTION_INPUT_I24,            /**<  DL12 data output CH1 */
    AUDIO_INTERCONNECTION_INPUT_I25,            /**<  DL12 data output CH2 */
    AUDIO_INTERCONNECTION_INPUT_I26,            /**<  SRC1 output CH1 */
    AUDIO_INTERCONNECTION_INPUT_I27,            /**<  SRC1 output CH2 */
    AUDIO_INTERCONNECTION_INPUT_I28,            /**<  SRC2 output CH1 */
    AUDIO_INTERCONNECTION_INPUT_I29,            /**<  SRC2 output CH2 */
    AUDIO_INTERCONNECTION_INPUT_I30,            /**<  I2S0 slave input CH1 */
    AUDIO_INTERCONNECTION_INPUT_I31,            /**<  I2S0 slave input CH2 */
    AUDIO_INTERCONNECTION_INPUT_I32,            /**<  I2S1 slave input CH1 */
    AUDIO_INTERCONNECTION_INPUT_I33,            /**<  I2S1 slave input CH2 */
    AUDIO_INTERCONNECTION_INPUT_I34,            /**<  I2S2 slave input CH1 */
    AUDIO_INTERCONNECTION_INPUT_I35,            /**<  I2S2 slave input CH2 */
    AUDIO_INTERCONNECTION_INPUT_I36,            /**<  UL4 input CH1 */
    AUDIO_INTERCONNECTION_INPUT_I37,            /**<  UL4 input CH2 */
    AUDIO_INTERCONNECTION_INPUT_I38,            /**<  DnSample CH01 output CH1 */
    AUDIO_INTERCONNECTION_INPUT_I39,            /**<  DnSample CH01 output CH2 */
    AUDIO_INTERCONNECTION_INPUT_I40,            /**<  DnSample CH23 output CH1 */
    AUDIO_INTERCONNECTION_INPUT_I41,            /**<  DnSample CH23 output CH2 */
    AUDIO_INTERCONNECTION_INPUT_I42,            /**<  UpSample CH01 output CH1 */
    AUDIO_INTERCONNECTION_INPUT_I43,            /**<  UpSample CH01 output CH2 */
    AUDIO_INTERCONNECTION_INPUT_NUM
} hal_audio_path_interconnection_input_t;

/*Align AFE RG bit operation*/
typedef enum {
    AUDIO_INTERCONNECTION_OUTPUT_O00 = 0,       /**<  I2S0 master output CH1 */
    AUDIO_INTERCONNECTION_OUTPUT_O01,           /**<  I2S0 master output CH2 */
    AUDIO_INTERCONNECTION_OUTPUT_O02,           /**<  I2S1 master output CH1 */
    AUDIO_INTERCONNECTION_OUTPUT_O03,           /**<  I2S1 master output CH2 */
    AUDIO_INTERCONNECTION_OUTPUT_O04,           /**<  I2S2 master output CH1 */
    AUDIO_INTERCONNECTION_OUTPUT_O05,           /**<  I2S2 master output CH2 */
    AUDIO_INTERCONNECTION_OUTPUT_O06,           /**<  HW Gain3 input CH1 */
    AUDIO_INTERCONNECTION_OUTPUT_O07,           /**<  HW Gain3 input CH2 */
    AUDIO_INTERCONNECTION_OUTPUT_O08,           /**<  DL1 output CH1 */
    AUDIO_INTERCONNECTION_OUTPUT_O09,           /**<  DL1 output CH2 */
    AUDIO_INTERCONNECTION_OUTPUT_O10,           /**<  sidetone input CH1 */
    AUDIO_INTERCONNECTION_OUTPUT_O11,           /**<  sidetone input CH2 */
    AUDIO_INTERCONNECTION_OUTPUT_O12,           /**<  HW Gain1 input CH1 */
    AUDIO_INTERCONNECTION_OUTPUT_O13,           /**<  HW Gain1 input CH2 */
    AUDIO_INTERCONNECTION_OUTPUT_O14,           /**<  HW Gain2 input CH1 */
    AUDIO_INTERCONNECTION_OUTPUT_O15,           /**<  HW Gain2 input CH2 */
    AUDIO_INTERCONNECTION_OUTPUT_O16,           /**<  VUL1 data input CH1 */
    AUDIO_INTERCONNECTION_OUTPUT_O17,           /**<  VUL1 data input CH2 */
    AUDIO_INTERCONNECTION_OUTPUT_O18,           /**<  VUL2 data input CH1 */
    AUDIO_INTERCONNECTION_OUTPUT_O19,           /**<  VUL2 data input CH2 */
    AUDIO_INTERCONNECTION_OUTPUT_O20,           /**<  AWB data input CH1 */
    AUDIO_INTERCONNECTION_OUTPUT_O21,           /**<  AWB data input CH2 */
    AUDIO_INTERCONNECTION_OUTPUT_O22,           /**<  AWB2 data input CH1 */
    AUDIO_INTERCONNECTION_OUTPUT_O23,           /**<  AWB2 data input CH2 */
    AUDIO_INTERCONNECTION_OUTPUT_O24,           /**<  VUL3 data input CH1 */
    AUDIO_INTERCONNECTION_OUTPUT_O25,           /**<  VUL3 data input CH2 */
    AUDIO_INTERCONNECTION_OUTPUT_O26,           /**<  I2S0 slave output CH1 */
    AUDIO_INTERCONNECTION_OUTPUT_O27,           /**<  I2S0 slave output CH2 */
    AUDIO_INTERCONNECTION_OUTPUT_O28,           /**<  I2S1 slave output CH1 */
    AUDIO_INTERCONNECTION_OUTPUT_O29,           /**<  I2S1 slave output CH2 */
    AUDIO_INTERCONNECTION_OUTPUT_O30,           /**<  I2S2 slave output CH1 */
    AUDIO_INTERCONNECTION_OUTPUT_O31,           /**<  I2S2 slave output CH2 */
    AUDIO_INTERCONNECTION_OUTPUT_O32,           /**<  DnSample CH01 input CH1 */
    AUDIO_INTERCONNECTION_OUTPUT_O33,           /**<  DnSample CH01 input CH2 */
    AUDIO_INTERCONNECTION_OUTPUT_O34,           /**<  DnSample CH23 input CH1 */
    AUDIO_INTERCONNECTION_OUTPUT_O35,           /**<  DnSample CH23 input CH2 */
    AUDIO_INTERCONNECTION_OUTPUT_O36,           /**<  UpSample CH01 input CH1 */
    AUDIO_INTERCONNECTION_OUTPUT_O37,           /**<  UpSample CH01 input CH2 */

    AUDIO_INTERCONNECTION_OUTPUT_NUM
} hal_audio_path_interconnection_output_t;

typedef enum {
    AUDIO_INTERCONNECTION_DISCONNECT    = 0x0,
    AUDIO_INTERCONNECTION_CONNECT       = 0x1,
    AUDIO_INTERCONNECTION_CONNECTSHIFT  = 0x2, /**<  Right shift 1 bit */
} hal_audio_path_interconnection_state_t;

typedef enum {
    HAL_AUDIO_PATH_DATA_FORMAT_16BIT = 0,
    HAL_AUDIO_PATH_DATA_FORMAT_24BIT,
} hal_audio_path_output_data_format_t;


typedef enum {
    HAL_AUDIO_PATH_CHANNEL_CH01CH02_to_CH01CH02     = HAL_AUDIO_INTERCONN_CH01CH02_to_CH01CH02,
    HAL_AUDIO_PATH_CHANNEL_CH01CH02_to_CH02CH01     = HAL_AUDIO_INTERCONN_CH01CH02_to_CH02CH01,
    HAL_AUDIO_PATH_CHANNEL_MIX                      = HAL_AUDIO_INTERCONN_MIX,
    HAL_AUDIO_PATH_CHANNEL_MIX_SHIFT_RIGHT          = HAL_AUDIO_INTERCONN_MIX_SHIFT_RIGHT,

    HAL_AUDIO_PATH_CHANNEL_CH01_to_CH01CH02         = HAL_AUDIO_INTERCONN_CH01_to_CH01CH02,
    HAL_AUDIO_PATH_CHANNEL_CH01_to_CH01             = HAL_AUDIO_INTERCONN_CH01_to_CH01,
    HAL_AUDIO_PATH_CHANNEL_CH01_to_CH02             = HAL_AUDIO_INTERCONN_CH01_to_CH02,

    HAL_AUDIO_PATH_CHANNEL_SOURCE_ONLY_CH2          = HAL_AUDIO_INTERCONN_CH02_to_CH01CH02,
    HAL_AUDIO_PATH_CHANNEL_CH02_to_CH01CH02         = HAL_AUDIO_INTERCONN_CH02_to_CH01CH02,
    HAL_AUDIO_PATH_CHANNEL_CH02_to_CH01             = HAL_AUDIO_INTERCONN_CH02_to_CH01,
    HAL_AUDIO_PATH_CHANNEL_CH02_to_CH02             = HAL_AUDIO_INTERCONN_CH02_to_CH02,

    HAL_AUDIO_PATH_CHANNEL_DIRECT,
    HAL_AUDIO_PATH_CHANNEL_NUM,
} hal_audio_path_channel_t;

typedef enum {
    HAL_AUDIO_PATH_TICK_ALIGN_OFF            = 0,
    HAL_AUDIO_PATH_TICK_SOURCE_I2S0_IN       = 1,
    HAL_AUDIO_PATH_TICK_SOURCE_I2S1_IN       = 2,
    HAL_AUDIO_PATH_TICK_SOURCE_I2S2_IN       = 3,
    HAL_AUDIO_PATH_TICK_SOURCE_I2S0_OUT      = 4,
    HAL_AUDIO_PATH_TICK_SOURCE_I2S1_OUT      = 5,
    HAL_AUDIO_PATH_TICK_SOURCE_I2S2_OUT      = 6,

    HAL_AUDIO_PATH_TICK_SOURCE_DOWN_SAMPLER_OUTPUT_CH0  = 7,
    HAL_AUDIO_PATH_TICK_SOURCE_DOWN_SAMPLER_OUTPUT_CH2  = 8,
} hal_audio_path_interconnection_tick_source_t;



typedef struct {
    hal_audio_path_interconnection_input_t input_port : 16;
    int16_t duplicate_count;
} hal_audio_path_user_counter_t;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Variables ///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function Prototypes ////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool hal_audio_path_set_connection(hal_audio_path_parameter_t *handle, hal_audio_control_status_t control);
#ifdef  AIR_AUDIO_HW_LOOPBACK_ENABLE
void audio_hw_loopback_echo_enable(bool enable);
#endif
hal_audio_path_interconnection_input_t hal_audio_path_get_input_interconnection_port(hal_audio_interconn_selection_t interconn_select);
hal_audio_path_interconnection_output_t hal_audio_path_get_output_interconnection_port(hal_audio_interconn_selection_t interconn_select);
bool hal_audio_path_set_interconnection(hal_audio_path_interconnection_state_t connection_state, hal_audio_path_channel_t connection_channel, hal_audio_path_interconnection_input_t input, hal_audio_path_interconnection_output_t output);



#endif //#ifdef HAL_AUDIO_MODULE_ENABLED
#endif /* __HAL_AUDIO_PATH_H__ */
