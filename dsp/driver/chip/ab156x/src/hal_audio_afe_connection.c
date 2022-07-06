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

#include "hal_audio.h"

#ifdef HAL_AUDIO_MODULE_ENABLED

#include "hal_audio_afe_define.h"
#include "hal_audio_afe_connection.h"
#include "common.h"
#include "hal_log.h"

typedef bool (*connection_function_t)(uint32_t);

struct connection_link_t {
    uint32_t input;
    uint32_t output;
    connection_function_t connection_function;
};

struct connection_link_table_t {
    audio_afe_io_block_t io;
    uint32_t connection;
};



/* afe interconnection register */
static const uint32_t afe_connection_reg[AUDIO_INTERCONNECTION_OUTPUT_NUM] = {
    AFE_CONN0,  AFE_CONN1,  AFE_CONN2,  AFE_CONN3,  AFE_CONN4,
    AFE_CONN5,  AFE_CONN6,  AFE_CONN7,  AFE_CONN8,  AFE_CONN9,
    AFE_CONN10, AFE_CONN11, AFE_CONN12, AFE_CONN13, AFE_CONN14,
    AFE_CONN15, AFE_CONN16, AFE_CONN17, AFE_CONN18, AFE_CONN19,
    AFE_CONN20, AFE_CONN21, AFE_CONN22, AFE_CONN23
};
#if 0//modify for ab1568

/* connection state of register */
static uint8_t afe_connection_state[AUDIO_INTERCONNECTION_INPUT_NUM][AUDIO_INTERCONNECTION_OUTPUT_NUM] = { {0} };


/*16/24 bits*/
static bool afe_set_output_connection_format(afe_output_data_format_t connection_format, audio_interconnection_output_t output)
{
    AFE_SET_REG(AFE_CONN_24BIT, connection_format << output, 1 << output);
    return true;
}

static bool afe_set_connection_state(audio_interconnection_state_t connection_state, audio_interconnection_input_t input, audio_interconnection_output_t output)
{
    uint32_t connect_reg = afe_connection_reg[output];
    uint32_t set_bit = input;
    //log_hal_msgid_info("DSP set connect state:%d, in:%d, out:%d \r\n", 3, connection_state, input, output);
    switch (connection_state) {
        case AUDIO_INTERCONNECTION_DISCONNECT:
        {
            if ((afe_connection_state[input][output] & AUDIO_INTERCONNECTION_CONNECT) == AUDIO_INTERCONNECTION_CONNECT) {
                AFE_SET_REG(connect_reg, 0, 1 << set_bit);
                afe_connection_state[input][output] &= ~(AUDIO_INTERCONNECTION_CONNECT);
            }
            if ((afe_connection_state[input][output] & AUDIO_INTERCONNECTION_CONNECTSHIFT) == AUDIO_INTERCONNECTION_CONNECTSHIFT) {
                AFE_SET_REG(AFE_CONN_RS, 0, 1 << output);
                afe_connection_state[input][output] &= ~(AUDIO_INTERCONNECTION_CONNECTSHIFT);
            }
            break;
        }
        case AUDIO_INTERCONNECTION_CONNECTSHIFT: // Call case: connect and connectshift sequentially if want o/p data = data/2
        {
            AFE_SET_REG(AFE_CONN_RS, 1 << output, 1 << output);
            afe_connection_state[input][output] |= AUDIO_INTERCONNECTION_CONNECTSHIFT;
        }
        case AUDIO_INTERCONNECTION_CONNECT:
        {
            AFE_SET_REG(connect_reg, 1 << set_bit, 1 << set_bit);
            afe_connection_state[input][output] |= AUDIO_INTERCONNECTION_CONNECT;
            break;
        }
        default:
            break;
    }
    return true;
}
#endif

#if 0
static bool afe_set_dl1_to_dac(audio_interconnection_state_t connection_state)
{
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I18, AUDIO_INTERCONNECTION_OUTPUT_O08);
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I19,	AUDIO_INTERCONNECTION_OUTPUT_O09);
    return true;
}

/* DL R=L */
static bool afe_set_dl1_ch1_to_dac(audio_interconnection_state_t connection_state)
{
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I18, AUDIO_INTERCONNECTION_OUTPUT_O08);
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I18, AUDIO_INTERCONNECTION_OUTPUT_O09);
    return true;
}

static bool afe_set_dl1_ch1_to_dac_ch1(audio_interconnection_state_t connection_state)
{
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I18, AUDIO_INTERCONNECTION_OUTPUT_O08);
    return true;
}

static bool afe_set_dl1_to_dac_ch1(audio_interconnection_state_t connection_state)
{
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I18, AUDIO_INTERCONNECTION_OUTPUT_O08);
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I19, AUDIO_INTERCONNECTION_OUTPUT_O08);
    return true;
}

/* DL(stereo)->UL(stereo) loopback */
static bool afe_set_dl1_to_vul1(audio_interconnection_state_t connection_state)
{
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I18, AUDIO_INTERCONNECTION_OUTPUT_O16);
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I19, AUDIO_INTERCONNECTION_OUTPUT_O17);
    return true;
}

static bool afe_set_dl1_ch1_to_vul1_ch1(audio_interconnection_state_t connection_state)
{
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I18, AUDIO_INTERCONNECTION_OUTPUT_O16);
    return true;
}

static bool afe_set_dl1_to_vul2(audio_interconnection_state_t connection_state)
{
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I18, AUDIO_INTERCONNECTION_OUTPUT_O18);
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I19, AUDIO_INTERCONNECTION_OUTPUT_O19);
    return true;
}

static bool afe_set_adc_to_vul1(audio_interconnection_state_t connection_state)
{
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I08, AUDIO_INTERCONNECTION_OUTPUT_O16);
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I09, AUDIO_INTERCONNECTION_OUTPUT_O17);
    return true;
}

static bool afe_set_adc_lch_to_vul1_lch(audio_interconnection_state_t connection_state)
{
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I08, AUDIO_INTERCONNECTION_OUTPUT_O16);
    return true;
}

/* mono UL -> UL R&L */
static bool afe_set_adc_lch_to_vul1(audio_interconnection_state_t connection_state)
{
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I08, AUDIO_INTERCONNECTION_OUTPUT_O16);
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I08, AUDIO_INTERCONNECTION_OUTPUT_O17);
    return true;
}

static bool afe_set_dl1_lch_to_i2s0_lch(audio_interconnection_state_t connection_state)
{
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I18, AUDIO_INTERCONNECTION_OUTPUT_O00);
    return true;
}

static bool afe_set_dl1_lch_to_i2s0(audio_interconnection_state_t connection_state)
{
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I18, AUDIO_INTERCONNECTION_OUTPUT_O00);
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I18, AUDIO_INTERCONNECTION_OUTPUT_O01);
    return true;
}

static bool afe_set_dl1_to_i2s0_lch(audio_interconnection_state_t connection_state)
{
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I18, AUDIO_INTERCONNECTION_OUTPUT_O00);
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I19, AUDIO_INTERCONNECTION_OUTPUT_O00);
    return true;
}

static bool afe_set_dl1_to_i2s0(audio_interconnection_state_t connection_state)
{
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I18, AUDIO_INTERCONNECTION_OUTPUT_O00);
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I19, AUDIO_INTERCONNECTION_OUTPUT_O01);
    return true;
}

static bool afe_set_i2s0_lch_to_vul1_lch(audio_interconnection_state_t connection_state)
{
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I00, AUDIO_INTERCONNECTION_OUTPUT_O16);
    return true;
}

static bool afe_set_i2s0_lch_to_vul1(audio_interconnection_state_t connection_state)
{
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I00, AUDIO_INTERCONNECTION_OUTPUT_O16);
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I00, AUDIO_INTERCONNECTION_OUTPUT_O17);
    return true;
}

static bool afe_set_i2s0_to_vul1(audio_interconnection_state_t connection_state)
{
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I00, AUDIO_INTERCONNECTION_OUTPUT_O16);
    afe_set_connection_state(connection_state, AUDIO_INTERCONNECTION_INPUT_I01, AUDIO_INTERCONNECTION_OUTPUT_O17);
    return true;
}


static const struct connection_link_t afe_connection_link[] = {
    {AUDIO_AFE_IO_BLOCK_MEM_DL1,     AUDIO_AFE_IO_BLOCK_ADDA_DL,      afe_set_dl1_to_dac         },
    {AUDIO_AFE_IO_BLOCK_MEM_DL1_CH1, AUDIO_AFE_IO_BLOCK_ADDA_DL1_LCH, afe_set_dl1_ch1_to_dac_ch1 },
    {AUDIO_AFE_IO_BLOCK_MEM_DL1_CH1, AUDIO_AFE_IO_BLOCK_ADDA_DL,      afe_set_dl1_ch1_to_dac     },
    {AUDIO_AFE_IO_BLOCK_MEM_DL1,     AUDIO_AFE_IO_BLOCK_ADDA_DL1_LCH, afe_set_dl1_to_dac_ch1     },
    {AUDIO_AFE_IO_BLOCK_MEM_DL1,     AUDIO_AFE_IO_BLOCK_MEM_VUL1,     afe_set_dl1_to_vul1        },
    {AUDIO_AFE_IO_BLOCK_MEM_DL1_CH1, AUDIO_AFE_IO_BLOCK_MEM_VUL1_CH1, afe_set_dl1_ch1_to_vul1_ch1},
    {AUDIO_AFE_IO_BLOCK_MEM_DL1,     AUDIO_AFE_IO_BLOCK_MEM_VUL2,     afe_set_dl1_to_vul2        },
    {AUDIO_AFE_IO_BLOCK_ADDA_UL1,    AUDIO_AFE_IO_BLOCK_MEM_VUL1,     afe_set_adc_to_vul1        },
    {AUDIO_AFE_IO_BLOCK_ADDA_UL1_LCH,AUDIO_AFE_IO_BLOCK_MEM_VUL1_CH1, afe_set_adc_lch_to_vul1_lch},
    {AUDIO_AFE_IO_BLOCK_MEM_DL1_CH1, AUDIO_AFE_IO_BLOCK_I2S0_CH1_OUT, afe_set_dl1_lch_to_i2s0_lch },
    {AUDIO_AFE_IO_BLOCK_MEM_DL1_CH1, AUDIO_AFE_IO_BLOCK_I2S0_OUT,     afe_set_dl1_lch_to_i2s0     },
    {AUDIO_AFE_IO_BLOCK_MEM_DL1,     AUDIO_AFE_IO_BLOCK_I2S0_CH1_OUT, afe_set_dl1_to_i2s0_lch     },
    {AUDIO_AFE_IO_BLOCK_MEM_DL1,     AUDIO_AFE_IO_BLOCK_I2S0_OUT,     afe_set_dl1_to_i2s0         },
    {AUDIO_AFE_IO_BLOCK_ADDA_UL1_LCH,AUDIO_AFE_IO_BLOCK_MEM_VUL1,     afe_set_adc_lch_to_vul1     },
    {AUDIO_AFE_IO_BLOCK_I2S0_CH1_IN, AUDIO_AFE_IO_BLOCK_MEM_VUL1_CH1, afe_set_i2s0_lch_to_vul1_lch},
    {AUDIO_AFE_IO_BLOCK_I2S0_CH1_IN, AUDIO_AFE_IO_BLOCK_MEM_VUL1,     afe_set_i2s0_lch_to_vul1    },
    {AUDIO_AFE_IO_BLOCK_I2S0_IN,     AUDIO_AFE_IO_BLOCK_MEM_VUL1,     afe_set_i2s0_to_vul1        },
};

static const uint32_t AFE_CONNECTION_LINK_NUM = ARRAY_SIZE(afe_connection_link);

static connection_function_t afe_get_connection_function(audio_afe_io_block_t audio_block_in, audio_afe_io_block_t audio_block_out)
{
    uint32_t i = 0;

    for (i = 0; i < AFE_CONNECTION_LINK_NUM; i++) {
        if ((afe_connection_link[i].input == audio_block_in) && (afe_connection_link[i].output == audio_block_out)) {
            return afe_connection_link[i].connection_function;
        }
    }
    return 0; // return null link function
}

bool hal_audio_afe_set_intf_connection_state(audio_interconnection_state_t connection_state, audio_afe_io_block_t audio_block_in, audio_afe_io_block_t audio_block_out)
{
    uint32_t ret = false;
    connection_function_t connection_function = afe_get_connection_function(audio_block_in, audio_block_out);

    if (connection_function == 0) {
        return ret;
    }
    return connection_function(connection_state);
}
#else

static const struct connection_link_table_t afe_connection_link_table[] = {

    {AUDIO_AFE_IO_BLOCK_I2S0_IN_CH1,         AUDIO_INTERCONNECTION_INPUT_I00},
    {AUDIO_AFE_IO_BLOCK_I2S0_IN_CH2,         AUDIO_INTERCONNECTION_INPUT_I01},
    {AUDIO_AFE_IO_BLOCK_I2S1_IN_CH1,         AUDIO_INTERCONNECTION_INPUT_I02},
    {AUDIO_AFE_IO_BLOCK_I2S1_IN_CH2,         AUDIO_INTERCONNECTION_INPUT_I03},
    {AUDIO_AFE_IO_BLOCK_I2S2_IN_CH1,         AUDIO_INTERCONNECTION_INPUT_I04},
    {AUDIO_AFE_IO_BLOCK_I2S2_IN_CH2,         AUDIO_INTERCONNECTION_INPUT_I05},
    {AUDIO_AFE_IO_BLOCK_I2S3_IN_CH1,         AUDIO_INTERCONNECTION_INPUT_I06},
    {AUDIO_AFE_IO_BLOCK_I2S3_IN_CH2,         AUDIO_INTERCONNECTION_INPUT_I07},
    {AUDIO_AFE_IO_BLOCK_ADDA_UL1_CH1,        AUDIO_INTERCONNECTION_INPUT_I08},
    {AUDIO_AFE_IO_BLOCK_ADDA_UL1_CH2,        AUDIO_INTERCONNECTION_INPUT_I09},
    {AUDIO_AFE_IO_BLOCK_ADDA_UL2_CH1,        AUDIO_INTERCONNECTION_INPUT_I10},
    {AUDIO_AFE_IO_BLOCK_ADDA_UL2_CH2,        AUDIO_INTERCONNECTION_INPUT_I11},
    {AUDIO_AFE_IO_BLOCK_ADDA_UL3_CH1,        AUDIO_INTERCONNECTION_INPUT_I12},
    {AUDIO_AFE_IO_BLOCK_ADDA_UL3_CH2,        AUDIO_INTERCONNECTION_INPUT_I13},
    {AUDIO_AFE_IO_BLOCK_HW_GAIN1_OUT_CH1,    AUDIO_INTERCONNECTION_INPUT_I14},
    {AUDIO_AFE_IO_BLOCK_HW_GAIN1_OUT_CH2,    AUDIO_INTERCONNECTION_INPUT_I15},
    {AUDIO_AFE_IO_BLOCK_HW_GAIN2_OUT_CH1,    AUDIO_INTERCONNECTION_INPUT_I16},
    {AUDIO_AFE_IO_BLOCK_HW_GAIN2_OUT_CH2,    AUDIO_INTERCONNECTION_INPUT_I17},
    {AUDIO_AFE_IO_BLOCK_MEM_DL1_CH1,         AUDIO_INTERCONNECTION_INPUT_I18},
    {AUDIO_AFE_IO_BLOCK_MEM_DL1_CH2,         AUDIO_INTERCONNECTION_INPUT_I19},
    {AUDIO_AFE_IO_BLOCK_MEM_DL2_CH1,         AUDIO_INTERCONNECTION_INPUT_I20},
    {AUDIO_AFE_IO_BLOCK_MEM_DL2_CH2,         AUDIO_INTERCONNECTION_INPUT_I21},
    {AUDIO_AFE_IO_BLOCK_MEM_DL3_CH1,         AUDIO_INTERCONNECTION_INPUT_I22},
    {AUDIO_AFE_IO_BLOCK_MEM_DL3_CH2,         AUDIO_INTERCONNECTION_INPUT_I23},
    {AUDIO_AFE_IO_BLOCK_MEM_DL12_CH1,        AUDIO_INTERCONNECTION_INPUT_I24},
    {AUDIO_AFE_IO_BLOCK_MEM_DL12_CH2,        AUDIO_INTERCONNECTION_INPUT_I25},


    {AUDIO_AFE_IO_BLOCK_I2S0_OUT_CH1,       AUDIO_INTERCONNECTION_OUTPUT_O00},
    {AUDIO_AFE_IO_BLOCK_I2S0_OUT_CH2,       AUDIO_INTERCONNECTION_OUTPUT_O01},
    {AUDIO_AFE_IO_BLOCK_I2S1_OUT_CH1,       AUDIO_INTERCONNECTION_OUTPUT_O02},
    {AUDIO_AFE_IO_BLOCK_I2S1_OUT_CH2,       AUDIO_INTERCONNECTION_OUTPUT_O03},
    {AUDIO_AFE_IO_BLOCK_I2S2_OUT_CH1,       AUDIO_INTERCONNECTION_OUTPUT_O04},
    {AUDIO_AFE_IO_BLOCK_I2S2_OUT_CH2,       AUDIO_INTERCONNECTION_OUTPUT_O05},
    {AUDIO_AFE_IO_BLOCK_I2S3_OUT_CH1,       AUDIO_INTERCONNECTION_OUTPUT_O06},
    {AUDIO_AFE_IO_BLOCK_I2S3_OUT_CH2,       AUDIO_INTERCONNECTION_OUTPUT_O07},
    {AUDIO_AFE_IO_BLOCK_ADDA_DL_CH1,        AUDIO_INTERCONNECTION_OUTPUT_O08},
    {AUDIO_AFE_IO_BLOCK_ADDA_DL_CH2,        AUDIO_INTERCONNECTION_OUTPUT_O09},
    {AUDIO_AFE_IO_BLOCK_STF_CH1,            AUDIO_INTERCONNECTION_OUTPUT_O10},
    {AUDIO_AFE_IO_BLOCK_STF_CH2,            AUDIO_INTERCONNECTION_OUTPUT_O11},
    {AUDIO_AFE_IO_BLOCK_HW_GAIN1_IN_CH1,    AUDIO_INTERCONNECTION_OUTPUT_O12},
    {AUDIO_AFE_IO_BLOCK_HW_GAIN1_IN_CH2,    AUDIO_INTERCONNECTION_OUTPUT_O13},
    {AUDIO_AFE_IO_BLOCK_HW_GAIN2_IN_CH1,    AUDIO_INTERCONNECTION_OUTPUT_O14},
    {AUDIO_AFE_IO_BLOCK_HW_GAIN2_IN_CH2,    AUDIO_INTERCONNECTION_OUTPUT_O15},
    {AUDIO_AFE_IO_BLOCK_MEM_VUL1_CH1,       AUDIO_INTERCONNECTION_OUTPUT_O16},
    {AUDIO_AFE_IO_BLOCK_MEM_VUL1_CH2,       AUDIO_INTERCONNECTION_OUTPUT_O17},
    {AUDIO_AFE_IO_BLOCK_MEM_VUL2_CH1,       AUDIO_INTERCONNECTION_OUTPUT_O18},
    {AUDIO_AFE_IO_BLOCK_MEM_VUL2_CH2,       AUDIO_INTERCONNECTION_OUTPUT_O19},
    {AUDIO_AFE_IO_BLOCK_MEM_AWB_CH1,        AUDIO_INTERCONNECTION_OUTPUT_O20},
    {AUDIO_AFE_IO_BLOCK_MEM_AWB_CH2,        AUDIO_INTERCONNECTION_OUTPUT_O21},
    {AUDIO_AFE_IO_BLOCK_MEM_AWB2_CH1,       AUDIO_INTERCONNECTION_OUTPUT_O22},
    {AUDIO_AFE_IO_BLOCK_MEM_AWB2_CH2,       AUDIO_INTERCONNECTION_OUTPUT_O23},
};

static const uint32_t AFE_CONNECTION_LINK_TABLE_NUM = ARRAY_SIZE(afe_connection_link_table);
#if 0//modify for ab1568
bool hal_set_afe_connection_function (audio_interconnection_state_t connection_state, afe_stream_channel_t stream_channel, audio_interconnection_input_t input, audio_interconnection_output_t output)
{
    afe_set_connection_state(connection_state, input, output);
    switch(stream_channel) {
        case STREAM_M_AFE_M:
        default:
            break;
        case STREAM_S_AFE_M:
            afe_set_connection_state(connection_state, input+1, output);
            break;
        case STREAM_M_AFE_S:
            afe_set_connection_state(connection_state, input, output+1);
            break;
        case STREAM_S_AFE_S:
            if (output%2) {
                // SWAP
                afe_set_connection_state(connection_state, input+1, output-1);
            } else {
                afe_set_connection_state(connection_state, input+1, output+1);
            }
            break;
        case STREAM_B_AFE_B:
            afe_set_connection_state(connection_state, input+1, output+1);
            afe_set_connection_state(connection_state, input+1, output);
            afe_set_connection_state(connection_state, input, output+1);
            break;
    }
    return true;
}

bool hal_audio_afe_set_intf_connection_state(audio_interconnection_state_t connection_state, afe_stream_channel_t stream_channel, audio_afe_io_block_t audio_block_in, audio_afe_io_block_t audio_block_out)
{
    audio_interconnection_input_t input;
    audio_interconnection_output_t output;
    uint32_t i, search_flag = 0;

    for (i = 0; i < AFE_CONNECTION_LINK_TABLE_NUM; i++) {
        if (afe_connection_link_table[i].io == audio_block_in) {
            input = afe_connection_link_table[i].connection;
            search_flag |= 0x01;
        }
        else if (afe_connection_link_table[i].io == audio_block_out) {
            output = afe_connection_link_table[i].connection;
            search_flag |= 0x02;
        }
        if (search_flag == 0x03) {
            return hal_set_afe_connection_function (connection_state, stream_channel, input, output);
        }
    }

    return false;
}
#endif

bool hal_audio_afe_set_connection(void *audio_param, bool is_input, bool enable)
{
    UNUSED(audio_param);
    UNUSED(is_input);
    UNUSED(enable);
    return false;
    #if 0//modify for ab1568
    AUDIO_PARAMETER *runtime = (AUDIO_PARAMETER *)audio_param;
    audio_afe_io_block_t audio_block_in = 0, audio_block_out = 0;
#ifdef ENABLE_2A2D_TEST
    audio_afe_io_block_t audio_block_in1 = 0;
    audio_afe_io_block_t audio_block_in2 = 0;
    audio_afe_io_block_t audio_block_in3 = 0;
#endif
    afe_stream_channel_t stream_channel;
    audio_interconnection_state_t connection_state;
    audio_digital_block_t digital_block = 0;
    connection_state = (enable) ? AUDIO_INTERCONNECTION_CONNECT : AUDIO_INTERCONNECTION_DISCONNECT;
    stream_channel = runtime->connect_channel_type;
    if (is_input) {
        // Input connection
        if (runtime->memory == HAL_AUDIO_MEM1) {
            audio_block_out = AUDIO_AFE_IO_BLOCK_MEM_VUL1;
            digital_block = AUDIO_DIGITAL_BLOCK_MEM_VUL1;
        } else if (runtime->memory == HAL_AUDIO_MEM2) {
            audio_block_out = AUDIO_AFE_IO_BLOCK_MEM_VUL2;
            digital_block = AUDIO_DIGITAL_BLOCK_MEM_VUL2;
        } else if (runtime->memory == HAL_AUDIO_MEM3) {
            audio_block_out = AUDIO_AFE_IO_BLOCK_MEM_AWB;
            digital_block = AUDIO_DIGITAL_BLOCK_MEM_AWB;
        } else if (runtime->memory == HAL_AUDIO_MEM4) {
            audio_block_out = AUDIO_AFE_IO_BLOCK_MEM_AWB2;
            digital_block = AUDIO_DIGITAL_BLOCK_MEM_AWB2;
        } else {
            log_hal_msgid_warning("DSP set connect memory error :%d\r\n", 1, runtime->memory);
            audio_block_out = AUDIO_AFE_IO_BLOCK_MEM_VUL1;
        }

        if (runtime->audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
            if (runtime->audio_interface == HAL_AUDIO_INTERFACE_1) {
                audio_block_in = AUDIO_AFE_IO_BLOCK_I2S0_IN;
            } else if (runtime->audio_interface == HAL_AUDIO_INTERFACE_2) {
                audio_block_in = AUDIO_AFE_IO_BLOCK_I2S1_IN;
            } else if (runtime->audio_interface == HAL_AUDIO_INTERFACE_3) {
                audio_block_in = AUDIO_AFE_IO_BLOCK_I2S2_IN;
            } else if (runtime->audio_interface == HAL_AUDIO_INTERFACE_4) {
                audio_block_in = AUDIO_AFE_IO_BLOCK_I2S3_IN;
            }
        } else {
            audio_block_in = AUDIO_AFE_IO_BLOCK_ADDA_UL1;
            if (runtime->audio_interface == HAL_AUDIO_INTERFACE_1) {
                audio_block_in = AUDIO_AFE_IO_BLOCK_ADDA_UL1;
            } else if (runtime->audio_interface == HAL_AUDIO_INTERFACE_2) {
                audio_block_in = AUDIO_AFE_IO_BLOCK_ADDA_UL2;
            } else if (runtime->audio_interface == HAL_AUDIO_INTERFACE_3) {
                audio_block_in = AUDIO_AFE_IO_BLOCK_ADDA_UL3;
            }
        }
#ifdef ENABLE_2A2D_TEST
        if (runtime->audio_device1 == HAL_AUDIO_DEVICE_I2S_MASTER) {
            if (runtime->audio_interface1 == HAL_AUDIO_INTERFACE_1) {
                audio_block_in1 = AUDIO_AFE_IO_BLOCK_I2S0_IN;
            } else if (runtime->audio_interface1 == HAL_AUDIO_INTERFACE_2) {
                audio_block_in1 = AUDIO_AFE_IO_BLOCK_I2S1_IN;
            } else if (runtime->audio_interface1 == HAL_AUDIO_INTERFACE_3) {
                audio_block_in1 = AUDIO_AFE_IO_BLOCK_I2S2_IN;
            } else if (runtime->audio_interface1 == HAL_AUDIO_INTERFACE_4) {
                audio_block_in1 = AUDIO_AFE_IO_BLOCK_I2S3_IN;
            }
        } else {
            audio_block_in1 = AUDIO_AFE_IO_BLOCK_ADDA_UL1;
            if (runtime->audio_interface1 == HAL_AUDIO_INTERFACE_1) {
                audio_block_in1 = AUDIO_AFE_IO_BLOCK_ADDA_UL1;
            } else if (runtime->audio_interface1 == HAL_AUDIO_INTERFACE_2) {
                audio_block_in1 = AUDIO_AFE_IO_BLOCK_ADDA_UL2;
            } else if (runtime->audio_interface1 == HAL_AUDIO_INTERFACE_3) {
                audio_block_in1 = AUDIO_AFE_IO_BLOCK_ADDA_UL3;
            }
        }
        if (runtime->audio_device2 == HAL_AUDIO_DEVICE_I2S_MASTER) {
            if (runtime->audio_interface2 == HAL_AUDIO_INTERFACE_1) {
                audio_block_in2 = AUDIO_AFE_IO_BLOCK_I2S0_IN;
            } else if (runtime->audio_interface2 == HAL_AUDIO_INTERFACE_2) {
                audio_block_in2 = AUDIO_AFE_IO_BLOCK_I2S1_IN;
            } else if (runtime->audio_interface2 == HAL_AUDIO_INTERFACE_3) {
                audio_block_in2 = AUDIO_AFE_IO_BLOCK_I2S2_IN;
            } else if (runtime->audio_interface2 == HAL_AUDIO_INTERFACE_4) {
                audio_block_in2 = AUDIO_AFE_IO_BLOCK_I2S3_IN;
            }
        } else {
            audio_block_in2 = AUDIO_AFE_IO_BLOCK_ADDA_UL1;
            if (runtime->audio_interface2 == HAL_AUDIO_INTERFACE_1) {
                audio_block_in2 = AUDIO_AFE_IO_BLOCK_ADDA_UL1;
            } else if (runtime->audio_interface2 == HAL_AUDIO_INTERFACE_2) {
                audio_block_in2 = AUDIO_AFE_IO_BLOCK_ADDA_UL2;
            } else if (runtime->audio_interface2 == HAL_AUDIO_INTERFACE_3) {
                audio_block_in2 = AUDIO_AFE_IO_BLOCK_ADDA_UL3;
            }
        }
        if (runtime->audio_device3 == HAL_AUDIO_DEVICE_I2S_MASTER) {
            if (runtime->audio_interface3 == HAL_AUDIO_INTERFACE_1) {
                audio_block_in3 = AUDIO_AFE_IO_BLOCK_I2S0_IN;
            } else if (runtime->audio_interface3 == HAL_AUDIO_INTERFACE_2) {
                audio_block_in3 = AUDIO_AFE_IO_BLOCK_I2S1_IN;
            } else if (runtime->audio_interface3 == HAL_AUDIO_INTERFACE_3) {
                audio_block_in3 = AUDIO_AFE_IO_BLOCK_I2S2_IN;
            } else if (runtime->audio_interface3 == HAL_AUDIO_INTERFACE_4) {
                audio_block_in3 = AUDIO_AFE_IO_BLOCK_I2S3_IN;
            }
        } else {
            audio_block_in3 = AUDIO_AFE_IO_BLOCK_ADDA_UL1;
            if (runtime->audio_interface3 == HAL_AUDIO_INTERFACE_1) {
                audio_block_in3 = AUDIO_AFE_IO_BLOCK_ADDA_UL1;
            } else if (runtime->audio_interface3 == HAL_AUDIO_INTERFACE_2) {
                audio_block_in3 = AUDIO_AFE_IO_BLOCK_ADDA_UL2;
            } else if (runtime->audio_interface3 == HAL_AUDIO_INTERFACE_3) {
                audio_block_in3 = AUDIO_AFE_IO_BLOCK_ADDA_UL3;
            }
        }
#endif
    } else {
        // output connection

        if (runtime->memory == HAL_AUDIO_MEM1) {
            audio_block_in = AUDIO_AFE_IO_BLOCK_MEM_DL1;
            digital_block = AUDIO_DIGITAL_BLOCK_MEM_DL1;
        } else if (runtime->memory == HAL_AUDIO_MEM2) {
            audio_block_in = AUDIO_AFE_IO_BLOCK_MEM_DL2;
            digital_block = AUDIO_DIGITAL_BLOCK_MEM_DL2;
        } else if (runtime->memory == HAL_AUDIO_MEM3) {
            audio_block_in = AUDIO_AFE_IO_BLOCK_MEM_DL3;
            digital_block = AUDIO_DIGITAL_BLOCK_MEM_DL3;
        } else if (runtime->memory == HAL_AUDIO_MEM4) {
            audio_block_in = AUDIO_AFE_IO_BLOCK_MEM_DL12;
            digital_block = AUDIO_DIGITAL_BLOCK_MEM_DL12;
        } else {
            log_hal_msgid_warning("DSP set connect memory error :%d\r\n", 1, runtime->memory);
            audio_block_in = AUDIO_AFE_IO_BLOCK_MEM_DL1;
        }

        if (runtime->audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
            if (runtime->audio_interface == HAL_AUDIO_INTERFACE_1) {
                audio_block_out = AUDIO_AFE_IO_BLOCK_I2S0_OUT;
            } else if (runtime->audio_interface == HAL_AUDIO_INTERFACE_2) {
                audio_block_out = AUDIO_AFE_IO_BLOCK_I2S1_OUT;
            } else if (runtime->audio_interface == HAL_AUDIO_INTERFACE_3) {
                audio_block_out = AUDIO_AFE_IO_BLOCK_I2S2_OUT;
            } else if (runtime->audio_interface == HAL_AUDIO_INTERFACE_4) {
                audio_block_out = AUDIO_AFE_IO_BLOCK_I2S3_OUT;
            }
        } else {
            audio_block_out = AUDIO_AFE_IO_BLOCK_ADDA_DL;
        }
    }

    if (runtime->format == AFE_PCM_FORMAT_S32_LE || runtime->format == AFE_PCM_FORMAT_U32_LE) {
        hal_audio_afe_set_connection_format(AFE_OUTPUT_DATA_FORMAT_24BIT, audio_block_out);
        afe_set_memif_fetch_format_per_sample(digital_block, AFE_WLEN_32_BIT_ALIGN_24BIT_DATA_8BIT_0);
    } else if (runtime->format == AFE_PCM_FORMAT_S16_LE || runtime->format == AFE_PCM_FORMAT_U16_LE) {
        hal_audio_afe_set_connection_format(AFE_OUTPUT_DATA_FORMAT_16BIT, audio_block_out);
        afe_set_memif_fetch_format_per_sample(digital_block, AFE_WLEN_16_BIT);
    } else {
        log_hal_msgid_warning("DSP format error %d\r\n", 1, runtime->format);
    }

    if (runtime->hw_gain) {
        /*HW gain connection*/
        if (audio_block_in == AUDIO_AFE_IO_BLOCK_MEM_DL1) {
            hal_audio_afe_set_intf_connection_state(connection_state, STREAM_S_AFE_S, audio_block_in, AUDIO_AFE_IO_BLOCK_HW_GAIN1_IN);
            audio_block_in = AUDIO_AFE_IO_BLOCK_HW_GAIN1_OUT;
        } else if (audio_block_in == AUDIO_AFE_IO_BLOCK_MEM_DL2) {
            hal_audio_afe_set_intf_connection_state(connection_state, STREAM_S_AFE_S, audio_block_in, AUDIO_AFE_IO_BLOCK_HW_GAIN2_IN);
            audio_block_in = AUDIO_AFE_IO_BLOCK_HW_GAIN2_OUT;
        }
        if (runtime->format == AFE_PCM_FORMAT_S32_LE || runtime->format == AFE_PCM_FORMAT_U32_LE) {
            hal_audio_afe_set_connection_format(AFE_OUTPUT_DATA_FORMAT_24BIT, audio_block_in);
        } else  {
            hal_audio_afe_set_connection_format(AFE_OUTPUT_DATA_FORMAT_16BIT, audio_block_in);
        }
    }

    if (runtime->stream_channel == HAL_AUDIO_BOTH_R) {
        audio_block_in++;
        stream_channel = STREAM_M_AFE_S;
    } else if (runtime->stream_channel == HAL_AUDIO_BOTH_L) {
        stream_channel = STREAM_M_AFE_S;
    } else if (runtime->stream_channel == HAL_AUDIO_SWAP_L_R) {
        audio_block_out++;
    } else if (runtime->stream_channel == HAL_AUDIO_MIX_L_R) {
        stream_channel = STREAM_B_AFE_B;
    } else if (runtime->stream_channel == HAL_AUDIO_MIX_SHIFT_L_R) {
        connection_state = (enable) ? AUDIO_INTERCONNECTION_CONNECTSHIFT : AUDIO_INTERCONNECTION_DISCONNECT;
        stream_channel = STREAM_B_AFE_B;
    } else if (runtime->stream_channel == HAL_AUDIO_DIRECT) {
        if(runtime->audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_R ||
           runtime->audio_device == HAL_AUDIO_DEVICE_DIGITAL_MIC_R ||
           runtime->audio_device == HAL_AUDIO_DEVICE_LINEINPLAYBACK_R){
            audio_block_in++;
        }
        if((runtime->audio_device == HAL_AUDIO_DEVICE_DAC_R)) {
            audio_block_out++;
        }
#ifdef ENABLE_2A2D_TEST
        if (runtime->audio_device1 != HAL_AUDIO_DEVICE_NONE) {
           //1A1D
           stream_channel = STREAM_M_AFE_M;
           if(runtime->audio_device1 == HAL_AUDIO_DEVICE_MAIN_MIC_R ||
              runtime->audio_device1 == HAL_AUDIO_DEVICE_DIGITAL_MIC_R ||
              runtime->audio_device1 == HAL_AUDIO_DEVICE_LINEINPLAYBACK_R){
               audio_block_in1++;
           }
           hal_audio_afe_set_intf_connection_state(connection_state, stream_channel, audio_block_in1, AUDIO_AFE_IO_BLOCK_MEM_VUL1+1);
           audio_block_out = AUDIO_AFE_IO_BLOCK_MEM_VUL1;
        }
        if (runtime->audio_device2 != HAL_AUDIO_DEVICE_NONE) {
            //2A1D
            if(runtime->audio_device2 == HAL_AUDIO_DEVICE_MAIN_MIC_R ||
               runtime->audio_device2 == HAL_AUDIO_DEVICE_DIGITAL_MIC_R ||
               runtime->audio_device2 == HAL_AUDIO_DEVICE_LINEINPLAYBACK_R){
                audio_block_in2++;
            }
            hal_audio_afe_set_intf_connection_state(connection_state, stream_channel, audio_block_in2, AUDIO_AFE_IO_BLOCK_MEM_VUL2);
        }
        if (runtime->audio_device3 != HAL_AUDIO_DEVICE_NONE) {
            //2A2D
            if(runtime->audio_device3 == HAL_AUDIO_DEVICE_MAIN_MIC_R ||
               runtime->audio_device3 == HAL_AUDIO_DEVICE_DIGITAL_MIC_R ||
               runtime->audio_device3 == HAL_AUDIO_DEVICE_LINEINPLAYBACK_R){
                audio_block_in3++;
            }
            hal_audio_afe_set_intf_connection_state(connection_state, stream_channel, audio_block_in3, AUDIO_AFE_IO_BLOCK_MEM_VUL2+1);
        }
#endif
    } else {
    }

    return hal_audio_afe_set_intf_connection_state(connection_state, stream_channel, audio_block_in, audio_block_out);
    #endif
}


audio_digital_block_t hal_audio_afe_get_memory_digital_block (hal_audio_memory_t memory, bool is_downlink)
{
    audio_digital_block_t memory_block;
    uint32_t offset;
    switch (memory) {
        case HAL_AUDIO_MEM1:
        default:
            offset = 0;
            break;
        case HAL_AUDIO_MEM2:
            offset = 1;
            break;
        case HAL_AUDIO_MEM3:
            offset = 2;
            break;
        case HAL_AUDIO_MEM4:
            offset = 3;
            break;
    }
    if (is_downlink) {
        //AUDIO_DIGITAL_BLOCK_MEM_DL1,
        //AUDIO_DIGITAL_BLOCK_MEM_DL2,
        //AUDIO_DIGITAL_BLOCK_MEM_DL3,
        //AUDIO_DIGITAL_BLOCK_MEM_DL12,
        memory_block = AUDIO_DIGITAL_BLOCK_MEM_DL1 + offset;
    } else {
        //AUDIO_DIGITAL_BLOCK_MEM_VUL1,
        //AUDIO_DIGITAL_BLOCK_MEM_VUL2,
        //AUDIO_DIGITAL_BLOCK_MEM_AWB,
        //AUDIO_DIGITAL_BLOCK_MEM_AWB2,
        memory_block = AUDIO_DIGITAL_BLOCK_MEM_VUL1 + offset;
    }
    return memory_block;
}



#endif
bool hal_audio_afe_set_connection_format(afe_output_data_format_t connection_format, audio_afe_io_block_t audio_block)
{
#if 1//modify for ab1568
    UNUSED(connection_format);
    UNUSED(audio_block);
    AFE_WRITE(AFE_CONN_24BIT, 0xFFFFFFFF);
    return true;
#else
    switch (audio_block) {
        case AUDIO_AFE_IO_BLOCK_I2S0_OUT:
        {
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O00);
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O01);
            break;
        }
        case AUDIO_AFE_IO_BLOCK_I2S1_OUT:
        {
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O02);
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O03);
            break;
        }
        case AUDIO_AFE_IO_BLOCK_I2S2_OUT:
        {
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O04);
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O05);
            break;
        }
        case AUDIO_AFE_IO_BLOCK_I2S3_OUT:
        {
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O06);
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O07);
            break;
        }
        case AUDIO_AFE_IO_BLOCK_ADDA_DL:
        {
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O08);
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O09);
            break;
        }
        case AUDIO_AFE_IO_BLOCK_STF:
        {
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O10);
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O11);
            break;
        }
        case AUDIO_AFE_IO_BLOCK_HW_GAIN1_OUT:
        case AUDIO_AFE_IO_BLOCK_HW_GAIN1_IN:
        {
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O12);
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O13);
            break;
        }
        case AUDIO_AFE_IO_BLOCK_HW_GAIN2_OUT:
        case AUDIO_AFE_IO_BLOCK_HW_GAIN2_IN:
        {
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O14);
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O15);
            break;
        }
        case AUDIO_AFE_IO_BLOCK_MEM_VUL1:
        {
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O16);
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O17);
            break;
        }
        case AUDIO_AFE_IO_BLOCK_MEM_VUL2:
        {
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O18);
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O19);
            break;
        }
        case AUDIO_AFE_IO_BLOCK_MEM_AWB:
        {
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O20);
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O21);
            break;
        }
        case AUDIO_AFE_IO_BLOCK_MEM_AWB2:
        {
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O22);
            afe_set_output_connection_format(connection_format, AUDIO_INTERCONNECTION_OUTPUT_O23);
            break;
        }
        default:
            break;
    }
    return true;
#endif
}

#endif //#ifdef HAL_AUDIO_MODULE_ENABLED
