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

#ifndef __HAL_AUDIO_VOLUME_H__
#define __HAL_AUDIO_VOLUME_H__

#include "hal_audio.h"
#include "hal_audio_control.h"

#ifdef HAL_AUDIO_MODULE_ENABLED

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define HAL_AUDIO_VOLUME_VALUE_CONVERT              (1)     //0:Remove volume value convert to reduce code size
#define AFE_HW_DIGITAL_GAIN_DEFAULT_SAMPLE_PER_STEP (4)

#define AFE_HW_DIGITAL_GAIN_0DB_REGISTER_VALUE      (524288)  //(2^19)  align AFE_GAIN1(2) target gain range
#define AFE_HW_DIGITAL_GAIN_NEG_2DB_REGISTER_VALUE  (416456)  //-2dB
#define AFE_HW_DIGITAL_GAIN_MAX_REGISTER_VALUE      (AFE_HW_DIGITAL_GAIN_0DB_REGISTER_VALUE)
#define AFE_HW_DIGITAL_GAIN_MIN_REGISTER_VALUE      (0)     //

#define AFE_HW_ANALOG_INPUT_0DB_REGISTER_VALUE      (0)     //  0db
#define AFE_HW_ANALOG_INPUT_MAX_REGISTER_VALUE      (9)     //+36db
#define AFE_HW_ANALOG_INPUT_MIN_REGISTER_VALUE      (0)     //  0db

#define AFE_HW_ANALOG_OUTPUT_0DB_REGISTER_VALUE     (8)     // 1db
#define AFE_HW_ANALOG_OUTPUT_MAX_REGISTER_VALUE     (16)    //+4db
#define AFE_HW_ANALOG_OUTPUT_MIN_REGISTER_VALUE     (0xFFFF)
#define AFE_HW_ANALOG_OUTPUT_CLASSD_MIN_VALUE       (AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSD_NEG_6_DB)
#define AFE_HW_ANALOG_OUTPUT_CLASSAB_MIN_VALUE      (AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_32_DB)


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Type Definitions ///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum {
    AFE_VOLUME_ANALOG_INPUT_GAIN_ACC10K_POS_0_DB    = 0,
    AFE_VOLUME_ANALOG_INPUT_GAIN_ACC10K_POS_3_DB    = 1,
    AFE_VOLUME_ANALOG_INPUT_GAIN_ACC10K_POS_6_DB    = 2,
    AFE_VOLUME_ANALOG_INPUT_GAIN_ACC10K_POS_9_DB    = 3,
    AFE_VOLUME_ANALOG_INPUT_GAIN_ACC10K_POS_12_DB   = 4,
    AFE_VOLUME_ANALOG_INPUT_GAIN_ACC10K_POS_15_DB   = 5,
    AFE_VOLUME_ANALOG_INPUT_GAIN_ACC10K_POS_18_DB   = 6,
    AFE_VOLUME_ANALOG_INPUT_GAIN_ACC10K_POS_24_DB   = 7,
    AFE_VOLUME_ANALOG_INPUT_GAIN_ACC10K_POS_30_DB   = 8,
    AFE_VOLUME_ANALOG_INPUT_GAIN_ACC10K_POS_36_DB   = 9,

    AFE_VOLUME_ANALOG_INPUT_GAIN_ACC20K_POS_0_DB    = 2,
    AFE_VOLUME_ANALOG_INPUT_GAIN_ACC20K_POS_3_DB    = 3,
    AFE_VOLUME_ANALOG_INPUT_GAIN_ACC20K_POS_6_DB    = 4,
    AFE_VOLUME_ANALOG_INPUT_GAIN_ACC20K_POS_9_DB    = 5,
    AFE_VOLUME_ANALOG_INPUT_GAIN_ACC20K_POS_12_DB   = 6,
    AFE_VOLUME_ANALOG_INPUT_GAIN_ACC20K_POS_18_DB   = 7,
    AFE_VOLUME_ANALOG_INPUT_GAIN_ACC20K_POS_24_DB   = 8,
    AFE_VOLUME_ANALOG_INPUT_GAIN_ACC20K_POS_30_DB   = 9,

    AFE_VOLUME_ANALOG_INPUT_GAIN_DCC_POS_0_DB       = 0,
    AFE_VOLUME_ANALOG_INPUT_GAIN_DCC_POS_6_DB       = 1,
    AFE_VOLUME_ANALOG_INPUT_GAIN_DCC_POS_12_DB      = 2,
    AFE_VOLUME_ANALOG_INPUT_GAIN_DCC_POS_18_DB      = 3,
    AFE_VOLUME_ANALOG_INPUT_GAIN_DCC_POS_24_DB      = 4,
    AFE_VOLUME_ANALOG_INPUT_GAIN_DCC_POS_30_DB      = 5,
} afe_volume_analog_input_gain_value_t;

typedef enum {
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSD_NEG_6_DB   = 1,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSD_NEG_3_DB   = 2,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSD_POS_0_DB   = 4,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSD_POS_3_DB   = 8,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSD_POS_6_DB   = 16,

    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_POS_12_DB = 0x000,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_POS_11_DB = 0x041,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_POS_10_DB = 0x082,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_POS_9_DB  = 0x0C3,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_POS_8_DB  = 0x104,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_POS_7_DB  = 0x145,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_POS_6_DB  = 0x186,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_POS_5_DB  = 0x1C7,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_POS_4_DB  = 0x208,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_POS_3_DB  = 0x249,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_POS_2_DB  = 0x28A,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_POS_1_DB  = 0x2CB,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_POS_0_DB  = 0x30C,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_1_DB  = 0x34D,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_2_DB  = 0x38E,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_3_DB  = 0x3CF,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_4_DB  = 0x410,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_5_DB  = 0x451,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_6_DB  = 0x492,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_7_DB  = 0x4D3,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_8_DB  = 0x514,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_9_DB  = 0x555,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_10_DB = 0x596,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_11_DB = 0x5D7,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_12_DB = 0x618,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_13_DB = 0x659,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_14_DB = 0x69A,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_15_DB = 0x6DB,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_16_DB = 0x71C,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_17_DB = 0x75D,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_18_DB = 0x79E,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_19_DB = 0x7DF,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_20_DB = 0x820,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_21_DB = 0x861,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_22_DB = 0x8A2,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_23_DB = 0x8E3,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_24_DB = 0x924,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_25_DB = 0x965,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_26_DB = 0x9A6,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_27_DB = 0x9E7,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_28_DB = 0xA28,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_29_DB = 0xA69,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_30_DB = 0xAAA,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_31_DB = 0xAEB,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_32_DB = 0xB2C,

    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_MASK_L_CH = 0x03F,
    AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_MASK_R_CH = 0xFC0,

} afe_volume_analog_output_gain_value_t;


typedef enum {
    AFE_VOLUME_MUTE_FRAMEWORK       = HAL_AUDIO_VOLUME_MUTE_FRAMEWORK,
    AFE_VOLUME_MUTE_ZERO_PADDING    = HAL_AUDIO_VOLUME_MUTE_ZERO_PADDING,
    AFE_VOLUME_MUTE_BLOCK_DISABLE   = HAL_AUDIO_VOLUME_MUTE_NUMBER,
    AFE_VOLUME_MUTE_CHANGE_DL_RATE,
} afe_volume_mute_control_t;

typedef struct {
    afe_volume_mute_control_t mute;
    uint32_t register_value;
    uint32_t sample_per_step;
    int32_t index;                  //Units:one hundred times db
    int32_t index_compensation;     //Units:one hundred times db
} afe_volume_digital_control_t;

typedef struct {
    hal_audio_analog_mdoe_t   analog_mode;
    afe_volume_mute_control_t mute;
    uint32_t register_value_l;
    uint32_t register_value_r;
    int32_t index_l;                //Units:one hundred times db
    int32_t index_r;                //Units:one hundred times db
#ifdef AIR_AUDIO_LR_OUT_ANALOG_GAIN_OFFSET_ENABLE
    int32_t index_offset_l;         //Units:one hundred times db
    int32_t index_offset_r;         //Units:one hundred times db
#endif
} afe_volume_analog_control_t;



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Variables ///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function Prototypes ////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void hal_audio_volume_init(void);
void hal_volume_set_analog_mode(afe_hardware_analog_gain_t gain_select, hal_audio_analog_mdoe_t mdoe);
hal_audio_analog_mdoe_t hal_volume_get_analog_mode(afe_hardware_analog_gain_t gain_select);

uint32_t afe_calculate_digital_gain_index(int32_t digital_gain_in_01unit_db, uint32_t digital_0db_register_value);

bool afe_volume_digital_set_mute(afe_hardware_digital_gain_t gain_select, afe_volume_mute_control_t mask, bool enable);
bool afe_volume_digital_set_gain_by_index(afe_hardware_digital_gain_t gain_select, int32_t index);
bool afe_volume_digital_set_gain_by_value(afe_hardware_digital_gain_t gain_select, uint32_t value);
bool afe_volume_digital_update(afe_hardware_digital_gain_t gain_select);
bool afe_volume_digital_set_compensation(afe_hardware_digital_gain_t gain_select, uint32_t index_compensation);
bool afe_volume_digital_set_ramp_step(afe_hardware_digital_gain_t gain_select, uint32_t sample_per_step);
uint32_t afe_volume_digital_get_ramp_step(afe_hardware_digital_gain_t gain_select);


bool afe_volume_analog_set_mute(afe_hardware_analog_gain_t gain_select, afe_volume_mute_control_t mask, bool enable);
bool afe_volume_analog_set_gain_by_index(afe_hardware_analog_gain_t gain_select, int32_t index_l, int32_t index_r);
bool afe_volume_analog_set_gain_by_value(afe_hardware_analog_gain_t gain_select, uint32_t value_l, uint32_t value_r);
bool afe_volume_analog_update(afe_hardware_analog_gain_t gain_select);
uint32_t afe_volume_analog_get_target_register_value(afe_hardware_analog_gain_t gain_select);
bool afe_volume_analog_ramp_output(uint32_t target_gain);

#ifdef AIR_AUDIO_LR_OUT_ANALOG_GAIN_OFFSET_ENABLE
bool afe_volume_analog_set_offset_gain_by_index(afe_hardware_analog_gain_t gain_select, int32_t index_l, int32_t index_r);
#endif


#endif //#ifdef HAL_AUDIO_MODULE_ENABLED
#endif /* __HAL_AUDIO_VOLUME_H__ */
