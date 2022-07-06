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

#include "hal_audio_path.h"
#include "hal_audio_register.h"
#include "hal_audio_control.h"
#include "hal_audio_clock.h"
#include "hal_audio_driver.h"

#ifdef HAL_AUDIO_MODULE_ENABLED

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables Declaration //////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* afe interconnection register */
static const uint32_t afe_connection_register_lsb[AUDIO_INTERCONNECTION_OUTPUT_NUM] = {
    AFE_CONN0,  AFE_CONN1,  AFE_CONN2,  AFE_CONN3,  AFE_CONN4,
    AFE_CONN5,  AFE_CONN6,  AFE_CONN7,  AFE_CONN8,  AFE_CONN9,
    AFE_CONN10, AFE_CONN11, AFE_CONN12, AFE_CONN13, AFE_CONN14,
    AFE_CONN15, AFE_CONN16, AFE_CONN17, AFE_CONN18, AFE_CONN19,
    AFE_CONN20, AFE_CONN21, AFE_CONN22, AFE_CONN23, AFE_CONN24,
    AFE_CONN25, AFE_CONN26, AFE_CONN27, AFE_CONN28, AFE_CONN29,
    AFE_CONN30, AFE_CONN31, AFE_CONN32, AFE_CONN33, AFE_CONN34,
    AFE_CONN35, AFE_CONN36, AFE_CONN37,
};
static const uint32_t afe_connection_register_msb[AUDIO_INTERCONNECTION_OUTPUT_NUM] = {
    AFE_CONN0_1,  AFE_CONN1_1,  AFE_CONN2_1,  AFE_CONN3_1,  AFE_CONN4_1,
    AFE_CONN5_1,  AFE_CONN6_1,  AFE_CONN7_1,  AFE_CONN8_1,  AFE_CONN9_1,
    AFE_CONN10_1, AFE_CONN11_1, AFE_CONN12_1, AFE_CONN13_1, AFE_CONN14_1,
    AFE_CONN15_1, AFE_CONN16_1, AFE_CONN17_1, AFE_CONN18_1, AFE_CONN19_1,
    AFE_CONN20_1, AFE_CONN21_1, AFE_CONN22_1, AFE_CONN23_1, AFE_CONN24_1,
    AFE_CONN25_1, AFE_CONN26_1, AFE_CONN27_1, AFE_CONN28_1, AFE_CONN29_1,
    AFE_CONN30_1, AFE_CONN31_1, AFE_CONN32_1, AFE_CONN33_1, AFE_CONN34_1,
    AFE_CONN35_1, AFE_CONN36_1, AFE_CONN37_1,
};

static const hal_audio_path_interconnection_input_t hal_audio_path_input_table[] = {
    AUDIO_INTERCONNECTION_INPUT_I08,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL1_CH1          = 32, */
    AUDIO_INTERCONNECTION_INPUT_I09,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL1_CH2          = 33, */
    AUDIO_INTERCONNECTION_INPUT_I10,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL2_CH1          = 34, */
    AUDIO_INTERCONNECTION_INPUT_I11,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL2_CH2          = 35, */
    AUDIO_INTERCONNECTION_INPUT_I12,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL3_CH1          = 36, */
    AUDIO_INTERCONNECTION_INPUT_I13,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL3_CH2          = 37, */
    AUDIO_INTERCONNECTION_INPUT_I36,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL4_CH1          = 38, */
    AUDIO_INTERCONNECTION_INPUT_I37,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL4_CH2          = 39, */
    AUDIO_INTERCONNECTION_INPUT_I00,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_MASTER_I2S0_CH1  = 40, */
    AUDIO_INTERCONNECTION_INPUT_I01,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_MASTER_I2S0_CH2  = 41, */
    AUDIO_INTERCONNECTION_INPUT_I02,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_MASTER_I2S1_CH1  = 42, */
    AUDIO_INTERCONNECTION_INPUT_I03,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_MASTER_I2S1_CH2  = 43, */
    AUDIO_INTERCONNECTION_INPUT_I04,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_MASTER_I2S2_CH1  = 44, */
    AUDIO_INTERCONNECTION_INPUT_I05,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_MASTER_I2S2_CH2  = 45, */
    AUDIO_INTERCONNECTION_INPUT_I30,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_SLAVE_I2S0_CH1   = 46, */
    AUDIO_INTERCONNECTION_INPUT_I31,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_SLAVE_I2S0_CH2   = 47, */
    AUDIO_INTERCONNECTION_INPUT_I32,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_SLAVE_I2S1_CH1   = 48, */
    AUDIO_INTERCONNECTION_INPUT_I33,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_SLAVE_I2S1_CH2   = 49, */
    AUDIO_INTERCONNECTION_INPUT_I34,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_SLAVE_I2S2_CH1   = 50, */
    AUDIO_INTERCONNECTION_INPUT_I35,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_SLAVE_I2S2_CH2   = 51, */
    AUDIO_INTERCONNECTION_INPUT_I18,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL1_CH1          = 52, */
    AUDIO_INTERCONNECTION_INPUT_I19,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL1_CH2          = 53, */
    AUDIO_INTERCONNECTION_INPUT_I20,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL2_CH1          = 54, */
    AUDIO_INTERCONNECTION_INPUT_I21,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL2_CH2          = 55, */
    AUDIO_INTERCONNECTION_INPUT_I22,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL3_CH1          = 56, */
    AUDIO_INTERCONNECTION_INPUT_I23,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL3_CH2          = 57, */
    AUDIO_INTERCONNECTION_INPUT_I24,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL12_CH1         = 58, */
    AUDIO_INTERCONNECTION_INPUT_I25,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL12_CH2         = 59, */
    AUDIO_INTERCONNECTION_INPUT_I26,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_SRC1_CH1         = 60, */
    AUDIO_INTERCONNECTION_INPUT_I27,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_SRC1_CH2         = 61, */
    AUDIO_INTERCONNECTION_INPUT_I28,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_SRC2_CH1         = 62, */
    AUDIO_INTERCONNECTION_INPUT_I29,    /* HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_SRC2_CH2         = 63, */
};

static const hal_audio_path_interconnection_output_t hal_audio_path_output_table[] = {
    AUDIO_INTERCONNECTION_OUTPUT_O08,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_DAC_CH1         =  0, */
    AUDIO_INTERCONNECTION_OUTPUT_O09,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_DAC_CH2         =  1, */
    AUDIO_INTERCONNECTION_OUTPUT_O00,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_MASTER_I2S0_CH1 =  2, */
    AUDIO_INTERCONNECTION_OUTPUT_O01,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_MASTER_I2S0_CH2 =  3, */
    AUDIO_INTERCONNECTION_OUTPUT_O02,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_MASTER_I2S1_CH1 =  4, */
    AUDIO_INTERCONNECTION_OUTPUT_O03,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_MASTER_I2S1_CH2 =  5, */
    AUDIO_INTERCONNECTION_OUTPUT_O04,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_MASTER_I2S2_CH1 =  6, */
    AUDIO_INTERCONNECTION_OUTPUT_O05,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_MASTER_I2S2_CH2 =  7, */
    AUDIO_INTERCONNECTION_OUTPUT_O26,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_SLAVE_I2S0_CH1  =  8, */
    AUDIO_INTERCONNECTION_OUTPUT_O27,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_SLAVE_I2S0_CH2  =  9, */
    AUDIO_INTERCONNECTION_OUTPUT_O28,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_SLAVE_I2S1_CH1  = 10, */
    AUDIO_INTERCONNECTION_OUTPUT_O29,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_SLAVE_I2S1_CH2  = 11, */
    AUDIO_INTERCONNECTION_OUTPUT_O30,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_SLAVE_I2S2_CH1  = 12, */
    AUDIO_INTERCONNECTION_OUTPUT_O31,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_SLAVE_I2S2_CH2  = 13, */
    AUDIO_INTERCONNECTION_OUTPUT_O10,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_SIDETONE_CH1    = 14, */
    AUDIO_INTERCONNECTION_OUTPUT_O11,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_SIDETONE_CH2    = 15, */
    AUDIO_INTERCONNECTION_OUTPUT_O16,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL1_CH1        = 16, */
    AUDIO_INTERCONNECTION_OUTPUT_O17,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL1_CH2        = 17, */
    AUDIO_INTERCONNECTION_OUTPUT_O18,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL2_CH1        = 18, */
    AUDIO_INTERCONNECTION_OUTPUT_O19,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL2_CH2        = 19, */
    AUDIO_INTERCONNECTION_OUTPUT_O24,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL3_CH1        = 20, */
    AUDIO_INTERCONNECTION_OUTPUT_O25,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL3_CH2        = 21, */
    AUDIO_INTERCONNECTION_OUTPUT_O20,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_AWB_CH1         = 22, */
    AUDIO_INTERCONNECTION_OUTPUT_O21,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_AWB_CH2         = 23, */
    AUDIO_INTERCONNECTION_OUTPUT_O22,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_AWB2_CH1        = 24, */
    AUDIO_INTERCONNECTION_OUTPUT_O23,   /* HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_AWB2_CH2        = 25, */
};

#if defined(MTK_MULTI_MIC_STREAM_ENABLE) || defined(MTK_ANC_SURROUND_MONITOR_ENABLE) || defined(AIR_WIRED_AUDIO_ENABLE) || defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
hal_audio_path_user_counter_t   hal_audio_path_memory_ul_duplicate[AUDIO_INTERCONNECTION_OUTPUT_O25-AUDIO_INTERCONNECTION_OUTPUT_O16+1];
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functiion Declaration //////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
hal_audio_path_user_counter_t *hal_audio_path_get_user_counter(hal_audio_path_interconnection_output_t output)
{
    hal_audio_path_user_counter_t *user_counter = NULL;
    UNUSED(output);
#if defined(MTK_MULTI_MIC_STREAM_ENABLE) || defined(MTK_ANC_SURROUND_MONITOR_ENABLE) || defined(AIR_WIRED_AUDIO_ENABLE) || defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
    if ((output >= AUDIO_INTERCONNECTION_OUTPUT_O16) && (output <= AUDIO_INTERCONNECTION_OUTPUT_O25)) {
        user_counter = &hal_audio_path_memory_ul_duplicate[output-AUDIO_INTERCONNECTION_OUTPUT_O16];
    }
#endif
    return user_counter;
}

bool hal_audio_path_set_interconnection_format(hal_audio_path_interconnection_state_t connection_state, hal_audio_path_output_data_format_t data_format, hal_audio_path_interconnection_output_t output_port)
{
#if 1
    UNUSED(connection_state);
    UNUSED(data_format);
    UNUSED(output_port);
    AFE_WRITE(AFE_CONN_24BIT,   0xFFFFFFFF);
    AFE_WRITE(AFE_CONN_24BIT_1, 0xFFFFFFFF);
#else
    uint32_t register_address = (output_port >= AUDIO_INTERCONNECTION_OUTPUT_O32) ? AFE_CONN_24BIT_1 : AFE_CONN_24BIT;
    uint32_t offset_bit;

    if (output_port >= AUDIO_INTERCONNECTION_OUTPUT_O32) {
        register_address = AFE_CONN_24BIT_1;
        offset_bit = (output_port - AUDIO_INTERCONNECTION_OUTPUT_O32) & (~1);
    } else {
        register_address = AFE_CONN_24BIT;
        offset_bit = (output_port) & (~1);
    }
    if (connection_state == AUDIO_INTERCONNECTION_DISCONNECT) {
        AFE_SET_REG(AFE_CONN_24BIT, 3<<offset_bit, 3<<offset_bit);
    } else {
        AFE_SET_REG(AFE_CONN_24BIT, (connection_format*3)<<offset_bit, 3<<offset_bit);
    }
#endif
    return false;
}

bool hal_audio_path_set_interconnection_state(hal_audio_path_interconnection_state_t connection_state, hal_audio_path_interconnection_input_t input, hal_audio_path_interconnection_output_t output)
{
    uint32_t right_shift_reg, connect_reg;
    uint32_t right_shift_bit, connect_bit;
    hal_audio_path_user_counter_t *user_counter = NULL;
    bool is_duplicate_connect = false;
    user_counter = hal_audio_path_get_user_counter(output);

    hal_audio_path_set_interconnection_format(connection_state, HAL_AUDIO_PATH_DATA_FORMAT_24BIT, output);
    if (input >= AUDIO_INTERCONNECTION_INPUT_I32) {


        connect_reg = afe_connection_register_msb[output];
        connect_bit = input - AUDIO_INTERCONNECTION_INPUT_I32;
    } else {
        connect_reg = afe_connection_register_lsb[output];
        connect_bit = input;
    }

    if (output >= AUDIO_INTERCONNECTION_OUTPUT_O32) {
        right_shift_reg = AFE_CONN_RS_1;
        right_shift_bit = output - AUDIO_INTERCONNECTION_OUTPUT_O32;
    } else {
        right_shift_reg = AFE_CONN_RS;
        right_shift_bit = output;
    }

    switch (connection_state) {
        case AUDIO_INTERCONNECTION_DISCONNECT:
            if ((user_counter) && ((user_counter->input_port==input) && user_counter->duplicate_count>0)) {
                user_counter->duplicate_count--;
                is_duplicate_connect = true;
            } else {
                AFE_SET_REG(connect_reg, 0, 1<<connect_bit);
                AFE_SET_REG(right_shift_reg, 0, 1<<right_shift_bit);
            }
            break;
        case AUDIO_INTERCONNECTION_CONNECTSHIFT: // Call case: connect and connectshift sequentially if want o/p data = data/2
            AFE_SET_REG(right_shift_reg, 1<<right_shift_bit, 1<<right_shift_bit);
        case AUDIO_INTERCONNECTION_CONNECT:
            if (user_counter) {
                if (AFE_GET_REG(connect_reg)&(1<<connect_bit)) {
                    user_counter->input_port = input;
                    user_counter->duplicate_count++;
                    is_duplicate_connect = true;
                }
            }
            AFE_SET_REG(connect_reg, 1<<connect_bit, 1<<connect_bit);
            break;
        default:
            break;
    }

    HAL_AUDIO_LOG_INFO("DSP - Hal Audio Path interconnection:%d, in:%d, out:%d, duplicated:%d", 4, connection_state, input, output, is_duplicate_connect);
    return true;
}

bool hal_audio_path_set_interconnection(hal_audio_path_interconnection_state_t connection_state, hal_audio_path_channel_t connection_channel, hal_audio_path_interconnection_input_t input, hal_audio_path_interconnection_output_t output)
{
    hal_audio_path_interconnection_state_t set_shift_state;
    hal_audio_path_interconnection_input_t select_input = (connection_channel >= HAL_AUDIO_PATH_CHANNEL_SOURCE_ONLY_CH2) ? input+1 : input;

    switch(connection_channel) {
        case HAL_AUDIO_PATH_CHANNEL_CH01CH02_to_CH01CH02:
            hal_audio_path_set_interconnection_state(connection_state, input, output);
            hal_audio_path_set_interconnection_state(connection_state, input+1, output+1);
            break;
        case HAL_AUDIO_PATH_CHANNEL_CH01CH02_to_CH02CH01:
            hal_audio_path_set_interconnection_state(connection_state, input+1, output);
            hal_audio_path_set_interconnection_state(connection_state, input, output+1);
            break;
        case HAL_AUDIO_PATH_CHANNEL_MIX:
            hal_audio_path_set_interconnection_state(connection_state, input, output);
            hal_audio_path_set_interconnection_state(connection_state, input+1, output+1);
            hal_audio_path_set_interconnection_state(connection_state, input+1, output);
            hal_audio_path_set_interconnection_state(connection_state, input, output+1);
            break;
        case HAL_AUDIO_PATH_CHANNEL_MIX_SHIFT_RIGHT:
            set_shift_state = (connection_state == AUDIO_INTERCONNECTION_DISCONNECT) ? AUDIO_INTERCONNECTION_DISCONNECT : AUDIO_INTERCONNECTION_CONNECTSHIFT;
            hal_audio_path_set_interconnection_state(set_shift_state, input, output);
            hal_audio_path_set_interconnection_state(set_shift_state, input+1, output+1);
            hal_audio_path_set_interconnection_state(set_shift_state, input+1, output);
            hal_audio_path_set_interconnection_state(set_shift_state, input, output+1);
            break;

        case HAL_AUDIO_PATH_CHANNEL_CH01_to_CH01CH02:
        case HAL_AUDIO_PATH_CHANNEL_CH02_to_CH01CH02:
            hal_audio_path_set_interconnection_state(connection_state, select_input, output);
            hal_audio_path_set_interconnection_state(connection_state, select_input, output+1);
            break;

        case HAL_AUDIO_PATH_CHANNEL_CH01_to_CH01:
        case HAL_AUDIO_PATH_CHANNEL_CH02_to_CH01:
            hal_audio_path_set_interconnection_state(connection_state, select_input, output);
            break;

        case HAL_AUDIO_PATH_CHANNEL_CH01_to_CH02:
        case HAL_AUDIO_PATH_CHANNEL_CH02_to_CH02:
            hal_audio_path_set_interconnection_state(connection_state, select_input, output+1);
            break;

        case HAL_AUDIO_PATH_CHANNEL_DIRECT:
            hal_audio_path_set_interconnection_state(connection_state, input, output);
            break;

        default:
            break;
    }
    return false;
}

#if 0

hal_audio_path_interconnection_input_t hal_audio_path_get_input_interconnection(hal_audio_path_selection_parameter_t *path_selection)
{
    hal_audio_path_interconnection_input_t interconnection_input = AUDIO_INTERCONNECTION_INPUT_NUM;
    switch (path_selection->port) {
        case HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_L:
        case HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_L:
        case HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_R:
        case HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_R:
        case HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL:
        case HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL:
            if (path_selection->parameters.audio_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1) {
                interconnection_input = AUDIO_INTERCONNECTION_INPUT_I08;
            } else if (path_selection->parameters.audio_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2) {
                interconnection_input = AUDIO_INTERCONNECTION_INPUT_I10;
            } else if (path_selection->parameters.audio_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3) {
                interconnection_input = AUDIO_INTERCONNECTION_INPUT_I12;
            } else if (path_selection->parameters.audio_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_4) {
                interconnection_input = AUDIO_INTERCONNECTION_INPUT_I36;
            }
            break;

        case HAL_AUDIO_CONTROL_DEVICE_LINE_IN_L:
        case HAL_AUDIO_CONTROL_DEVICE_LINE_IN_R:
        case HAL_AUDIO_CONTROL_DEVICE_LINE_IN_DUAL:
            interconnection_input = AUDIO_INTERCONNECTION_INPUT_I08;
            break;

        case HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER:
            if (path_selection->parameters.audio_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1) {
                interconnection_input = AUDIO_INTERCONNECTION_INPUT_I00;
            } else if (path_selection->parameters.audio_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2) {
                interconnection_input = AUDIO_INTERCONNECTION_INPUT_I02;
            } else if (path_selection->parameters.audio_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3) {
                interconnection_input = AUDIO_INTERCONNECTION_INPUT_I04;
            }
            break;

        case HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE:
            if (path_selection->parameters.audio_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1) {
                interconnection_input = AUDIO_INTERCONNECTION_INPUT_I30;
            } else if (path_selection->parameters.audio_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2) {
                interconnection_input = AUDIO_INTERCONNECTION_INPUT_I32;
            } else if (path_selection->parameters.audio_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3) {
                interconnection_input = AUDIO_INTERCONNECTION_INPUT_I34;
            }
            break;

        case HAL_AUDIO_CONTROL_MEMORY_INTERFACE:
            if (path_selection->parameters.memory_select == HAL_AUDIO_MEMORY_DL_DL1) {
                interconnection_input = AUDIO_INTERCONNECTION_INPUT_I18;
            } else if (path_selection->parameters.memory_select == HAL_AUDIO_MEMORY_DL_DL2) {
                interconnection_input = AUDIO_INTERCONNECTION_INPUT_I20;
            } else if (path_selection->parameters.memory_select == HAL_AUDIO_MEMORY_DL_DL3) {
                interconnection_input = AUDIO_INTERCONNECTION_INPUT_I22;
            } else if (path_selection->parameters.memory_select == HAL_AUDIO_MEMORY_DL_DL12) {
                interconnection_input = AUDIO_INTERCONNECTION_INPUT_I24;
            } else if (path_selection->parameters.memory_select == HAL_AUDIO_MEMORY_DL_SRC1) {
                interconnection_input = AUDIO_INTERCONNECTION_INPUT_I26;
            } else if (path_selection->parameters.memory_select == HAL_AUDIO_MEMORY_DL_SRC2) {
                interconnection_input = AUDIO_INTERCONNECTION_INPUT_I28;
            }
            break;

        default:

            break;
    }

    return interconnection_input;
}

hal_audio_path_interconnection_output_t hal_audio_path_get_output_interconnection(hal_audio_path_selection_parameter_t *path_selection)
{
    hal_audio_path_interconnection_output_t interconnection_output = AUDIO_INTERCONNECTION_OUTPUT_NUM;

    switch (path_selection->port) {

        case HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_L:
        case HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_R:
        case HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_DUAL:
            interconnection_output = AUDIO_INTERCONNECTION_OUTPUT_O08;
            break;

        case HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER:
        case HAL_AUDIO_CONTROL_DEVICE_SPDIF:
            if (path_selection->parameters.audio_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1) {
                interconnection_output = AUDIO_INTERCONNECTION_OUTPUT_O00;
            } else if (path_selection->parameters.audio_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2) {
                interconnection_output = AUDIO_INTERCONNECTION_OUTPUT_O02;
            } else if (path_selection->parameters.audio_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3) {
                interconnection_output = AUDIO_INTERCONNECTION_OUTPUT_O04;
            }
            break;

        case HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE:
            if (path_selection->parameters.audio_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1) {
                interconnection_output = AUDIO_INTERCONNECTION_OUTPUT_O26;
            } else if (path_selection->parameters.audio_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2) {
                interconnection_output = AUDIO_INTERCONNECTION_OUTPUT_O28;
            } else if (path_selection->parameters.audio_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3) {
                interconnection_output = AUDIO_INTERCONNECTION_OUTPUT_O30;
            }
            break;

        case HAL_AUDIO_CONTROL_MEMORY_INTERFACE:
            if (path_selection->parameters.memory_select == HAL_AUDIO_MEMORY_UL_VUL1) {
                interconnection_output = AUDIO_INTERCONNECTION_OUTPUT_O16;
            } else if (path_selection->parameters.memory_select == HAL_AUDIO_MEMORY_UL_VUL2) {
                interconnection_output = AUDIO_INTERCONNECTION_OUTPUT_O18;
            } else if (path_selection->parameters.memory_select == HAL_AUDIO_MEMORY_UL_VUL3) {
                interconnection_output = AUDIO_INTERCONNECTION_OUTPUT_O24;
            } else if (path_selection->parameters.memory_select == HAL_AUDIO_MEMORY_UL_AWB) {
                interconnection_output = AUDIO_INTERCONNECTION_OUTPUT_O20;
            } else if (path_selection->parameters.memory_select == HAL_AUDIO_MEMORY_UL_AWB2) {
                interconnection_output = AUDIO_INTERCONNECTION_OUTPUT_O22;
            }
            break;
        case HAL_AUDIO_CONTROL_DEVICE_SIDETONE:
            interconnection_output = AUDIO_INTERCONNECTION_OUTPUT_O10;
            break;
        default:

            break;
    }

    return interconnection_output;
}
#endif

hal_audio_path_interconnection_input_t hal_audio_path_get_input_interconnection_port(hal_audio_interconn_selection_t interconn_select)
{
    hal_audio_path_interconnection_input_t interconnection_input = AUDIO_INTERCONNECTION_INPUT_NUM;
    if (((int32_t)interconn_select >= HAL_AUDIO_INTERCONN_SELECT_INPUT_MIN) && ((int32_t)interconn_select <= HAL_AUDIO_INTERCONN_SELECT_INPUT_MAX)) {
        interconnection_input = hal_audio_path_input_table[interconn_select-HAL_AUDIO_INTERCONN_SELECT_INPUT_MIN];
    } else {
        HAL_AUDIO_LOG_ERROR("DSP - Error Hal Audio Path Port Input Wrong : %d !", 1, interconn_select);
        HAL_AUDIO_ASSERT(false);
    }
    return interconnection_input;
}


hal_audio_path_interconnection_output_t hal_audio_path_get_output_interconnection_port(hal_audio_interconn_selection_t interconn_select)
{
    hal_audio_path_interconnection_output_t interconnection_output = AUDIO_INTERCONNECTION_OUTPUT_NUM;
    if (((int32_t)interconn_select >= (int32_t)HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MIN) && ((int32_t)interconn_select <= (int32_t)HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MAX)) {
        interconnection_output = hal_audio_path_output_table[interconn_select-HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MIN];
    } else {
        HAL_AUDIO_LOG_ERROR("DSP - Error Hal Audio Path Port Output Wrong : %d !", 1, interconn_select);
        HAL_AUDIO_ASSERT(false);
    }
    return interconnection_output;
}

#ifdef  AIR_AUDIO_HW_LOOPBACK_ENABLE
void audio_hw_loopback_echo_enable(bool enable)
{
    hal_audio_path_interconnection_state_t connection_state = (enable == HAL_AUDIO_CONTROL_ON) ? AUDIO_INTERCONNECTION_CONNECT : AUDIO_INTERCONNECTION_DISCONNECT;
#ifdef AIR_DUAL_CHIP_NR_ON_MASTER_ENABLE
    hal_audio_path_set_interconnection(connection_state, HAL_AUDIO_PATH_CHANNEL_DIRECT, AUDIO_INTERCONNECTION_INPUT_I00, AUDIO_INTERCONNECTION_OUTPUT_O34);
    hal_audio_path_set_interconnection(connection_state, HAL_AUDIO_PATH_CHANNEL_DIRECT, AUDIO_INTERCONNECTION_INPUT_I01, AUDIO_INTERCONNECTION_OUTPUT_O35);
#else
    hal_audio_path_set_interconnection(connection_state, HAL_AUDIO_PATH_CHANNEL_DIRECT, AUDIO_INTERCONNECTION_INPUT_I00, AUDIO_INTERCONNECTION_OUTPUT_O01);
    hal_audio_path_set_interconnection(connection_state, HAL_AUDIO_PATH_CHANNEL_DIRECT, AUDIO_INTERCONNECTION_INPUT_I01, AUDIO_INTERCONNECTION_OUTPUT_O01);
#endif
    hal_audio_path_set_interconnection(connection_state, HAL_AUDIO_PATH_CHANNEL_DIRECT, AUDIO_INTERCONNECTION_INPUT_I14, AUDIO_INTERCONNECTION_OUTPUT_O01);
    hal_audio_path_set_interconnection(connection_state, HAL_AUDIO_PATH_CHANNEL_DIRECT, AUDIO_INTERCONNECTION_INPUT_I15, AUDIO_INTERCONNECTION_OUTPUT_O01);
    hal_audio_path_set_interconnection(connection_state, HAL_AUDIO_PATH_CHANNEL_DIRECT, AUDIO_INTERCONNECTION_INPUT_I16, AUDIO_INTERCONNECTION_OUTPUT_O01);
    hal_audio_path_set_interconnection(connection_state, HAL_AUDIO_PATH_CHANNEL_DIRECT, AUDIO_INTERCONNECTION_INPUT_I17, AUDIO_INTERCONNECTION_OUTPUT_O01);
    hal_audio_path_set_interconnection(connection_state, HAL_AUDIO_PATH_CHANNEL_DIRECT, AUDIO_INTERCONNECTION_INPUT_I06, AUDIO_INTERCONNECTION_OUTPUT_O01);
    hal_audio_path_set_interconnection(connection_state, HAL_AUDIO_PATH_CHANNEL_DIRECT, AUDIO_INTERCONNECTION_INPUT_I07, AUDIO_INTERCONNECTION_OUTPUT_O01);
}
#endif

bool hal_audio_path_set_connection(hal_audio_path_parameter_t *handle, hal_audio_control_status_t control)
{
    hal_audio_path_interconnection_input_t input_port;
    hal_audio_path_interconnection_output_t output_port;
    hal_audio_memory_selection_t input_port_memory_select;
    hal_audio_memory_selection_t output_port_memory_select;

    hal_audio_path_interconnection_state_t connection_state = (control==HAL_AUDIO_CONTROL_ON) ? AUDIO_INTERCONNECTION_CONNECT : AUDIO_INTERCONNECTION_DISCONNECT;
    hal_audio_path_interconnection_tick_source_t tick_source ;
    hal_audio_path_channel_t model_connection_channel = HAL_AUDIO_PATH_CHANNEL_DIRECT;
    uint32_t connection_sequence = 0;
    uint32_t connection_select = 0;
    bool with_i2s_slave_connection;
    if (control == HAL_AUDIO_CONTROL_ON) {
        hal_audio_afe_set_enable(control);
    }


    for (connection_sequence=0 ; connection_sequence<handle->connection_number ; connection_sequence++) {
        if (control) { // First-In-Last-Out
            connection_select = connection_sequence;
        } else {
            connection_select = handle->connection_number - connection_sequence - 1;
        }
        input_port  = hal_audio_path_get_input_interconnection_port ((hal_audio_interconn_selection_t)handle->input.interconn_sequence[connection_select]);
        output_port = hal_audio_path_get_output_interconnection_port((hal_audio_interconn_selection_t)handle->output.interconn_sequence[connection_select]);
        input_port_memory_select  = hal_audio_memory_convert_interconn_select_to_memory_selecct((hal_audio_interconn_selection_t)handle->input.interconn_sequence[connection_select]);
        output_port_memory_select = hal_audio_memory_convert_interconn_select_to_memory_selecct((hal_audio_interconn_selection_t)handle->output.interconn_sequence[connection_select]);
        if ((input_port>=AUDIO_INTERCONNECTION_INPUT_I30) && (input_port<=AUDIO_INTERCONNECTION_INPUT_I35)) {
            //I2S salve input
            tick_source = HAL_AUDIO_PATH_TICK_SOURCE_I2S0_IN + ((input_port-AUDIO_INTERCONNECTION_INPUT_I30)>>1);
            with_i2s_slave_connection = true;
        } else if ((output_port>=AUDIO_INTERCONNECTION_OUTPUT_O26) && (output_port<=AUDIO_INTERCONNECTION_OUTPUT_O31)) {
            //I2S salve output
            tick_source = HAL_AUDIO_PATH_TICK_SOURCE_I2S0_OUT + ((output_port-AUDIO_INTERCONNECTION_OUTPUT_O26)>>1);
            with_i2s_slave_connection = true;
        } else {
            tick_source = HAL_AUDIO_PATH_TICK_ALIGN_OFF;
            with_i2s_slave_connection = false;
        }

        if (handle->with_upwdown_sampler) {
            HAL_AUDIO_LOG_INFO("upwdown handle->audio_path_input_rate:%d handle->audio_path_output_rate:%d\r\n", 2, handle->audio_path_input_rate,handle->audio_path_output_rate);
            hal_audio_path_interconnection_output_t upwdown_sampler_input_port = AUDIO_INTERCONNECTION_OUTPUT_NUM;
            hal_audio_path_interconnection_input_t upwdown_sampler_output_port = AUDIO_INTERCONNECTION_INPUT_NUM;
            afe_updown_sampler_id_t updown_id;
            afe_updown_configuration_t updown_configuration;

            updown_configuration.input_rate = handle->audio_path_input_rate;
            updown_configuration.output_rate =handle->audio_path_output_rate;
            updown_configuration.tick_align = tick_source;
            updown_configuration.is_echo_configure_input = false;
            // disconnect up_down sample
            if (control == 0) {
                uint64_t dn_ch01_l = ((uint64_t)AFE_READ(AFE_CONN32_1))<<32|AFE_READ(AFE_CONN32);
                uint64_t dn_ch01_r = ((uint64_t)AFE_READ(AFE_CONN33_1))<<32|AFE_READ(AFE_CONN33);

                uint64_t dn_ch23_l = ((uint64_t)AFE_READ(AFE_CONN34_1))<<32|AFE_READ(AFE_CONN34);
                uint64_t dn_ch23_r = ((uint64_t)AFE_READ(AFE_CONN35_1))<<32|AFE_READ(AFE_CONN35);

                uint64_t up_ch01_l = ((uint64_t)AFE_READ(AFE_CONN36_1))<<32|AFE_READ(AFE_CONN36);
                uint64_t up_ch01_r = ((uint64_t)AFE_READ(AFE_CONN37_1))<<32|AFE_READ(AFE_CONN37);
                // check input port
                uint64_t bit_position = 1 << input_port;
                // HAL_AUDIO_LOG_INFO("[Interconn] dn_01 L:0x%x R:0x%x, dn_23 L:0x%x R:0x%x, up_01 L:0x%x R:0x%x", 6, dn_ch01_l, dn_ch01_r, dn_ch23_l, dn_ch23_r, up_ch01_l, up_ch01_r);
                if (bit_position & dn_ch23_r) { // First in Last out
                    upwdown_sampler_input_port = AUDIO_INTERCONNECTION_OUTPUT_O35;
                    upwdown_sampler_output_port = AUDIO_INTERCONNECTION_INPUT_I41;
                    updown_configuration.is_echo_configure_input = true;
                    updown_id = AFE_UPDOWN_SAMPLER_DOWN_CH23;
                } else if (bit_position & dn_ch01_r) {
                    upwdown_sampler_input_port = AUDIO_INTERCONNECTION_OUTPUT_O33;
                    upwdown_sampler_output_port = AUDIO_INTERCONNECTION_INPUT_I39;
                    updown_id = AFE_UPDOWN_SAMPLER_DOWN_CH01;
                } else if (bit_position & dn_ch01_l) {
                    upwdown_sampler_input_port = AUDIO_INTERCONNECTION_OUTPUT_O32;
                    upwdown_sampler_output_port = AUDIO_INTERCONNECTION_INPUT_I38;
                    updown_id = AFE_UPDOWN_SAMPLER_DOWN_CH01;
                } else if (bit_position & up_ch01_r) {
                    upwdown_sampler_input_port = AUDIO_INTERCONNECTION_OUTPUT_O37;
                    upwdown_sampler_output_port = AUDIO_INTERCONNECTION_INPUT_I42;
                    updown_id = AFE_UPDOWN_SAMPLER_UP_CH01;
                } else if (bit_position & up_ch01_l) {
                    upwdown_sampler_input_port = AUDIO_INTERCONNECTION_OUTPUT_O36;
                    upwdown_sampler_output_port = AUDIO_INTERCONNECTION_INPUT_I43;
                    updown_id = AFE_UPDOWN_SAMPLER_UP_CH01;
                }  else if (bit_position & dn_ch23_l) {
                    upwdown_sampler_input_port = AUDIO_INTERCONNECTION_OUTPUT_O34;
                    upwdown_sampler_output_port = AUDIO_INTERCONNECTION_INPUT_I40;
                    updown_configuration.is_echo_configure_input = true;
                    updown_id = AFE_UPDOWN_SAMPLER_DOWN_CH23;
                } else {
                    goto WITHOUT_DOWNSAMPLER;
                }
                hal_audio_updown_set_agent(&updown_configuration, updown_id, control);
                HAL_AUDIO_LOG_INFO("[interconn] disconnect updn id:%d, agent used:%d, input_port %d", 3, updown_id, hal_audio_updown_get_agent_count(updown_id), input_port);
                hal_audio_path_set_interconnection(connection_state, model_connection_channel, input_port, upwdown_sampler_input_port);
                input_port = upwdown_sampler_output_port;
            } else {
                // Note: Only support single connection. Multi channel should be implemented by user.
                if (handle->audio_path_input_rate >= handle->audio_path_output_rate) {
                    //Down sampler
                    updown_id = AFE_UPDOWN_SAMPLER_DOWN_CH01;
                    if ((hal_audio_updown_get_agent_count(updown_id)+control) > 2) { // each down sampler has only 2 channel
                        updown_id = AFE_UPDOWN_SAMPLER_DOWN_CH23;
                        if ((hal_audio_updown_get_agent_count(updown_id)+control) > 2) {
                            HAL_AUDIO_LOG_WARNING("[interconn] Warining: DownSampler single channel is not enough!!", 0);
                            goto WITHOUT_DOWNSAMPLER;
                        }
                        updown_configuration.is_echo_configure_input = true; // CH23_L is occupied by Echo Reference.
                        upwdown_sampler_input_port = AUDIO_INTERCONNECTION_OUTPUT_O34;
                        upwdown_sampler_output_port = AUDIO_INTERCONNECTION_INPUT_I40;
                        hal_tick_align_set_memory_agent(input_port_memory_select, HAL_AUDIO_PATH_TICK_SOURCE_DOWN_SAMPLER_OUTPUT_CH2, with_i2s_slave_connection&&(control==HAL_AUDIO_CONTROL_ON));
                    } else {
                        upwdown_sampler_input_port = AUDIO_INTERCONNECTION_OUTPUT_O32;
                        upwdown_sampler_output_port = AUDIO_INTERCONNECTION_INPUT_I38;
                        hal_tick_align_set_memory_agent(input_port_memory_select, HAL_AUDIO_PATH_TICK_SOURCE_DOWN_SAMPLER_OUTPUT_CH0, with_i2s_slave_connection&&(control==HAL_AUDIO_CONTROL_ON));
                    }

                } else {
                    //Up sampler
                    updown_id = AFE_UPDOWN_SAMPLER_UP_CH01;
                    upwdown_sampler_input_port = AUDIO_INTERCONNECTION_OUTPUT_O36;
                    upwdown_sampler_output_port = AUDIO_INTERCONNECTION_INPUT_I42;
                }
                hal_audio_updown_set_agent(&updown_configuration, updown_id, control);
                if ((hal_audio_updown_get_agent_count(updown_id)+control)%2) {
                    upwdown_sampler_input_port++;
                    upwdown_sampler_output_port++;
                }
                hal_audio_path_set_interconnection(connection_state, model_connection_channel, input_port, upwdown_sampler_input_port);
                input_port = upwdown_sampler_output_port;
            }
        }
WITHOUT_DOWNSAMPLER:

        if (handle->with_hw_gain) {
            hal_audio_path_interconnection_output_t hw_gain_input_port;
            hal_audio_path_interconnection_input_t hw_gain_output_port;
            afe_hardware_digital_gain_t gain_select;
            if (input_port_memory_select&HAL_AUDIO_MEMORY_DL_MASK) {
                gain_select = hal_audio_hardware_gain_get_selcet(input_port_memory_select);
            } else {
                gain_select = AFE_HW_DIGITAL_GAIN1;
            }
            hal_audio_hardware_gain_set_agent(gain_select, handle->audio_path_output_rate, control);
            hal_tick_align_set_hw_gain(gain_select, tick_source, with_i2s_slave_connection&&(control==HAL_AUDIO_CONTROL_ON));
            if (gain_select == AFE_HW_DIGITAL_GAIN1) {
                hw_gain_input_port = AUDIO_INTERCONNECTION_OUTPUT_O12;
                hw_gain_output_port = AUDIO_INTERCONNECTION_INPUT_I14;
            } else if (gain_select == AFE_HW_DIGITAL_GAIN2) {
                hw_gain_input_port = AUDIO_INTERCONNECTION_OUTPUT_O14;
                hw_gain_output_port = AUDIO_INTERCONNECTION_INPUT_I16;
            } else {
                hw_gain_input_port = AUDIO_INTERCONNECTION_OUTPUT_O06;
                hw_gain_output_port = AUDIO_INTERCONNECTION_INPUT_I06;
            }
            if ((hal_audio_hardware_gain_get_agent_count(gain_select)+control)%2) {
                hw_gain_input_port++;
                hw_gain_output_port++;
            }
            hal_audio_path_set_interconnection(connection_state, model_connection_channel, input_port, hw_gain_input_port);
            input_port = hw_gain_output_port;
            HAL_AUDIO_LOG_INFO("HW GAIN id:%d rate:%d\r\n",2,gain_select,handle->audio_path_output_rate);

        }
#ifdef AIR_AUDIO_MIXER_GAIN_ENABLE
        if ((output_port == AUDIO_INTERCONNECTION_OUTPUT_O08) ||
            (output_port == AUDIO_INTERCONNECTION_OUTPUT_O09)) {
            //For DAC output port
            //Use HW gain3 for mixer gain

            hal_audio_path_interconnection_output_t hw_gain_input_port;
            hal_audio_path_interconnection_input_t hw_gain_output_port;
            afe_hardware_digital_gain_t gain_select;
            gain_select = AFE_HW_DIGITAL_GAIN3;
            hal_audio_hardware_gain_set_agent(gain_select, handle->audio_path_output_rate, control);
            hal_tick_align_set_hw_gain(gain_select, tick_source, with_i2s_slave_connection&&(control==HAL_AUDIO_CONTROL_ON));

            if (control || (hal_audio_hardware_gain_get_agent_count(gain_select)<2)) {
                hw_gain_input_port = AUDIO_INTERCONNECTION_OUTPUT_O06;
                hw_gain_output_port = AUDIO_INTERCONNECTION_INPUT_I06;

                if (input_port%2) {
                    hw_gain_input_port++;
                    hw_gain_output_port++;
                }
                hal_audio_path_set_interconnection(connection_state, model_connection_channel, input_port, hw_gain_input_port);
                input_port = hw_gain_output_port;
            }
        }
#endif

        if (input_port_memory_select&(HAL_AUDIO_MEMORY_DL_MASK&(~HAL_AUDIO_MEMORY_DL_DL12))) {
            #if 1
            //Connect echo path to AWB2 data input with down sampler
            #if (HAL_AUDIO_PATH_ECHO_CONNECTION_MODE == 0)
            hal_audio_path_set_interconnection(connection_state, model_connection_channel, input_port, AUDIO_INTERCONNECTION_OUTPUT_O34+(input_port%2));

            afe_updown_configuration_t updown_configuration;
            updown_configuration.input_rate = handle->audio_path_output_rate;
            updown_configuration.tick_align = tick_source;
            updown_configuration.is_echo_configure_input = true;//modify for leo
            if (hal_audio_updown_set_agent(&updown_configuration, AFE_UPDOWN_SAMPLER_DOWN_CH23, control)) {
                hal_tick_align_set_memory_agent(HAL_AUDIO_MEMORY_UL_AWB2, HAL_AUDIO_PATH_TICK_SOURCE_DOWN_SAMPLER_OUTPUT_CH2, with_i2s_slave_connection&&(control==HAL_AUDIO_CONTROL_ON));
                hal_audio_path_set_interconnection(connection_state, model_connection_channel, AUDIO_INTERCONNECTION_INPUT_I40, AUDIO_INTERCONNECTION_OUTPUT_O22);
            }
            #endif
            hal_tick_align_set_memory_agent(input_port_memory_select, tick_source, with_i2s_slave_connection&&(control==HAL_AUDIO_CONTROL_ON));
            #endif
        } else if (output_port_memory_select&HAL_AUDIO_MEMORY_UL_MASK) {
            hal_tick_align_set_memory_agent(output_port_memory_select, (handle->with_upwdown_sampler)? HAL_AUDIO_PATH_TICK_SOURCE_DOWN_SAMPLER_OUTPUT_CH0 : tick_source, with_i2s_slave_connection&&(control==HAL_AUDIO_CONTROL_ON));
        }
        hal_audio_path_set_interconnection(connection_state, model_connection_channel, input_port, output_port);
        if ((handle->connection_number==1) && (handle->with_dl_deq_mixer)) {
            output_port = (output_port%2) ? output_port-1 : output_port+1;
            hal_audio_path_set_interconnection(connection_state, model_connection_channel, input_port, output_port);
        }
    }
    if (control == HAL_AUDIO_CONTROL_OFF) {
        hal_audio_afe_set_enable(control);
    }
    return false;
}

#endif //#ifdef HAL_AUDIO_MODULE_ENABLED
