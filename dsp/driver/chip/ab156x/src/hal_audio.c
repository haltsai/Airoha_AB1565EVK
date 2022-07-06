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

#include "hal_audio_afe_control.h"
#include "hal_audio_afe_define.h"
#include "hal_audio_afe_clock.h"

#if 1
//modify for ab1568
#include "hal_audio.h"
#include "hal_audio_path.h"
#include "hal_audio_control.h"
#include "hal_audio_clock.h"
//#include "hal_audio_volume.h"
/**
  * @ Initialize hal audio
  */
extern void hal_audio_volume_init(void);
void hal_audio_initialize(void)
{
#ifdef HAL_AUDIO_MODULE_ENABLED

    hal_audio_clock_initialize();
    hal_audio_irq_initialize();
    hal_memory_initialize_sram();
    hal_audio_control_initialize();
    hal_audio_volume_init();
#endif
}


/**
  * @ Set audio device
  * @ handle :
  * @ device :
  * @ control :
  * @ Retval: HAL_AUDIO_STATUS_OK if operation is successful, others if channel number is invalid
  */

hal_audio_status_t hal_audio_set_device(hal_audio_device_parameter_t *handle, hal_audio_control_t device, hal_audio_control_status_t control)
{
#ifdef HAL_AUDIO_MODULE_ENABLED
    if (device&(~HAL_AUDIO_CONTROL_MEMORY_INTERFACE)) {
        hal_audio_device_set_agent(handle, device, control);
    } else {
        return HAL_AUDIO_STATUS_INVALID_PARAMETER;
    }

    return HAL_AUDIO_STATUS_OK;
#else
    UNUSED(handle);
    UNUSED(device);
    UNUSED(control);
    return HAL_AUDIO_STATUS_ERROR;
#endif

}

/**
  * @ Set audio path
  * @ handle :
  * @ device :
  * @ control :
  * @ Retval: HAL_AUDIO_STATUS_OK if operation is successful, others if channel number is invalid
  */
hal_audio_status_t hal_audio_set_memory(hal_audio_memory_parameter_t *handle, hal_audio_control_t memory_interface, hal_audio_control_status_t control)
{
#ifdef HAL_AUDIO_MODULE_ENABLED
    if (memory_interface&(HAL_AUDIO_CONTROL_MEMORY_INTERFACE)) {
        hal_audio_memory_set_agent(handle, memory_interface, control);
    } else {
        return HAL_AUDIO_STATUS_INVALID_PARAMETER;
    }
    return HAL_AUDIO_STATUS_OK;
#else
    UNUSED(handle);
    UNUSED(memory_interface);
    UNUSED(control);
    return HAL_AUDIO_STATUS_ERROR;
#endif

}


/**
  * @ Set audio path
  * @ handle :
  * @ device :
  * @ control :
  * @ Retval: HAL_AUDIO_STATUS_OK if operation is successful, others if channel number is invalid
  */
hal_audio_status_t hal_audio_set_path(hal_audio_path_parameter_t *handle, hal_audio_control_status_t control)
{
#ifdef HAL_AUDIO_MODULE_ENABLED
    if (!hal_audio_path_set_connection(handle, control))
    {
        return HAL_AUDIO_STATUS_OK;
    }
    return HAL_AUDIO_STATUS_ERROR;
#else
    UNUSED(handle);
    UNUSED(control);
    return HAL_AUDIO_STATUS_ERROR;
#endif
}


/**
  * @ Get audio related status
  * @ handle :
  * @ command :
  * @ Retval: value
  */
uint32_t hal_audio_get_value(hal_audio_get_value_parameter_t *handle, hal_audio_get_value_command_t command)
{
#ifdef HAL_AUDIO_MODULE_ENABLED
    return hal_audio_control_get_value(handle, command);
#else
    UNUSED(handle);
    UNUSED(command);
    return HAL_AUDIO_STATUS_ERROR;
#endif
}


/**
  * @ Set audio related status
  * @ handle :
  * @ device :
  * @ control :
  * @ Retval: HAL_AUDIO_STATUS_OK if operation is successful, others if channel number is invalid
  */
hal_audio_status_t hal_audio_set_value(hal_audio_set_value_parameter_t *handle, hal_audio_set_value_command_t command)
{
#ifdef HAL_AUDIO_MODULE_ENABLED
    return hal_audio_control_set_value(handle, command);
#else
    UNUSED(handle);
    UNUSED(command);
    return HAL_AUDIO_STATUS_ERROR;
#endif

}

#endif

extern afe_t afe;

void hal_audio_init(void)
{
    afe_control_init();
    afe_sidetone_init();
    //set default device in case device is setting after app on. null ops would cause fatal error.
    #if 0//modify for ab1568
    afe.stream_in.audio_device = HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC;
    afe.stream_out.audio_device = HAL_AUDIO_DEVICE_HEADSET;
    #endif
    hal_audio_initialize();
}

/**
  * @ Control the audio output device
  * @ device: output device
  */
hal_audio_status_t hal_audio_set_stream_out_device(hal_audio_device_t device)
{
    afe.stream_out.audio_device = device;
    /*should coding for device switch during audio on*/
    /*should return invalid device setting if device is not support in AB155x*/
    return HAL_AUDIO_STATUS_OK;
}

/**
  * @ Control the audio input device
  * @ device: input device
  */
hal_audio_status_t hal_audio_set_stream_in_device(hal_audio_device_t device)
{
    afe.stream_in.audio_device = device;
    /*should coding for device switch during audio on*/
    /*should return invalid device setting if device is not support in AB155x*/
    return HAL_AUDIO_STATUS_OK;
}

/**
  * @ Query the audio output device
  */
hal_audio_device_t hal_audio_get_stream_out_device(void)
{
    return afe.stream_out.audio_device;
}

/**
  * @ Query the audio input device
  */
hal_audio_device_t hal_audio_get_stream_in_device(void)
{
    return afe.stream_in.audio_device;
}

/**
  * @ Updates the audio output volume
  * @ digital_volume_index: digital gain index
  * @ analog_volume_index : analog gain index
  */
void hal_audio_set_stream_out_volume(uint16_t digital_volume_index, uint16_t analog_volume_index)
{
    #if 0//modify for ab1568
    if (analog_volume_index != HAL_AUDIO_INVALID_ANALOG_GAIN_INDEX) {
        afe.stream_out.analog_gain_index  = (int32_t)((int16_t)analog_volume_index);
    }
    if (digital_volume_index != HAL_AUDIO_INVALID_DIGITAL_GAIN_INDEX) {
        afe.stream_out.digital_gain_index = (int32_t)((int16_t)digital_volume_index);
    }
    if (afe_get_dac_clock_status()) {
        afe_audio_set_output_analog_gain();
    }
    afe_audio_set_output_digital_gain(AUDIO_HW_GAIN);
    #endif
    hal_audio_volume_digital_gain_parameter_t           digital_gain;
    memset(&digital_gain,0,sizeof(hal_audio_volume_digital_gain_parameter_t));
    digital_gain.memory_select = HAL_AUDIO_MEMORY_DL_DL1;
    digital_gain.value = (uint32_t)((int32_t)((int16_t)digital_volume_index));
    HAL_AUDIO_LOG_INFO("volume1 digital_gain.value 0x%x,digital_volume_index 0x%x cast 0x%x\r\n",3,digital_gain.value,digital_volume_index,(uint32_t)((int32_t)((int16_t)digital_volume_index)));
    hal_audio_set_value((hal_audio_set_value_parameter_t*)&digital_gain, HAL_AUDIO_SET_VOLUME_HW_DIGITAL_GAIN);
    hal_audio_volume_analog_output_gain_parameter_t     analog_output_gain;
    memset(&analog_output_gain,0,sizeof(hal_audio_volume_analog_output_gain_parameter_t));
    analog_output_gain.value_l = (uint32_t)((int32_t)((int16_t)analog_volume_index));
    analog_output_gain.value_r = (uint32_t)((int32_t)((int16_t)analog_volume_index));
    hal_audio_set_value((hal_audio_set_value_parameter_t*)&analog_output_gain, HAL_AUDIO_SET_VOLUME_OUTPUT_ANALOG_GAIN);

}

/**
  * @ Updates the audio output volume
  * @ digital_volume_index: digital gain index
  * @ analog_volume_index : analog gain index
  */
void hal_audio_set_stream_out_volume2(uint16_t digital_volume_index, uint16_t analog_volume_index)
{
    #if 0
    if (analog_volume_index != HAL_AUDIO_INVALID_ANALOG_GAIN_INDEX) {
        afe.stream_out.analog_gain_index  = (int32_t)((int16_t)analog_volume_index);
    }
    if (digital_volume_index != HAL_AUDIO_INVALID_DIGITAL_GAIN_INDEX) {
        afe.stream_out.digital_gain_index2 = (int32_t)((int16_t)digital_volume_index);
    }
    if (afe_get_dac_clock_status()) {
        afe_audio_set_output_analog_gain();
    }
    afe_audio_set_output_digital_gain(AUDIO_HW_GAIN2);
    #endif
    hal_audio_volume_digital_gain_parameter_t           digital_gain;
    memset(&digital_gain,0,sizeof(hal_audio_volume_digital_gain_parameter_t));
    digital_gain.memory_select = HAL_AUDIO_MEMORY_DL_DL2;
    digital_gain.value = (uint32_t)((int32_t)((int16_t)digital_volume_index));
    HAL_AUDIO_LOG_INFO("volume2 digital_gain.value 0x%x,digital_volume_index 0x%x cast 0x%x\r\n",3,digital_gain.value,digital_volume_index,(uint32_t)((int32_t)((int16_t)digital_volume_index)));
    hal_audio_set_value((hal_audio_set_value_parameter_t *)&digital_gain, HAL_AUDIO_SET_VOLUME_HW_DIGITAL_GAIN);
    hal_audio_volume_analog_output_gain_parameter_t     analog_output_gain;
    memset(&analog_output_gain,0,sizeof(hal_audio_volume_analog_output_gain_parameter_t));
    analog_output_gain.value_l = (uint32_t)((int32_t)((int16_t)analog_volume_index));
    analog_output_gain.value_r = (uint32_t)((int32_t)((int16_t)analog_volume_index));
    hal_audio_set_value((hal_audio_set_value_parameter_t *)&analog_output_gain, HAL_AUDIO_SET_VOLUME_OUTPUT_ANALOG_GAIN);
}

/**
  * @ Updates the audio output volume
  * @ digital_volume_index: digital gain index
  * @ analog_volume_index : analog gain index
  */
void hal_audio_set_stream_out_volume3(uint16_t digital_volume_index, uint16_t analog_volume_index)
{
    #if 0//modify for ab1568
    if (analog_volume_index != HAL_AUDIO_INVALID_ANALOG_GAIN_INDEX) {
        afe.stream_out.analog_gain_index  = (int32_t)((int16_t)analog_volume_index);
    }
    if (digital_volume_index != HAL_AUDIO_INVALID_DIGITAL_GAIN_INDEX) {
        afe.stream_out.digital_gain_index = (int32_t)((int16_t)digital_volume_index);
    }
    if (afe_get_dac_clock_status()) {
        afe_audio_set_output_analog_gain();
    }
    afe_audio_set_output_digital_gain(AUDIO_HW_GAIN);
    #endif
    hal_audio_volume_digital_gain_parameter_t           digital_gain;
    memset(&digital_gain,0,sizeof(hal_audio_volume_digital_gain_parameter_t));
    digital_gain.memory_select = HAL_AUDIO_MEMORY_DL_DL3;
    digital_gain.value = (uint32_t)((int32_t)((int16_t)digital_volume_index));
    HAL_AUDIO_LOG_INFO("volume3 digital_gain.value 0x%x,digital_volume_index 0x%x cast 0x%x\r\n",3,digital_gain.value,digital_volume_index,(uint32_t)((int32_t)((int16_t)digital_volume_index)));
    hal_audio_set_value((hal_audio_set_value_parameter_t *)&digital_gain, HAL_AUDIO_SET_VOLUME_HW_DIGITAL_GAIN);
    hal_audio_volume_analog_output_gain_parameter_t     analog_output_gain;
    memset(&analog_output_gain,0,sizeof(hal_audio_volume_analog_output_gain_parameter_t));
    analog_output_gain.value_l = (uint32_t)((int32_t)((int16_t)analog_volume_index));
    analog_output_gain.value_r = (uint32_t)((int32_t)((int16_t)analog_volume_index));
    hal_audio_set_value((hal_audio_set_value_parameter_t*)&analog_output_gain, HAL_AUDIO_SET_VOLUME_OUTPUT_ANALOG_GAIN);
}

#ifdef AIR_AUDIO_LR_OUT_ANALOG_GAIN_OFFSET_ENABLE
/**
  * @ Updates the audio output offset volume
  * @ analog_volume_l_offset_index: analog gain l offset index
  * @ analog_volume_r_offset_index : analog gain r offset index
  */
void hal_audio_set_stream_out_offset_volume(uint16_t analog_volume_l_offset_index, uint16_t analog_volume_r_offset_index)
{
    hal_audio_volume_analog_output_gain_parameter_t     analog_output_gain;
    memset(&analog_output_gain,0,sizeof(hal_audio_volume_analog_output_gain_parameter_t));
    analog_output_gain.value_l = (uint32_t)((int32_t)((int16_t)analog_volume_l_offset_index));
    analog_output_gain.value_r = (uint32_t)((int32_t)((int16_t)analog_volume_r_offset_index));
    hal_audio_set_value((hal_audio_set_value_parameter_t*)&analog_output_gain, HAL_AUDIO_SET_VOLUME_OUTPUT_ANALOG_GAIN_OFFSET);
}
#endif

/**
  * @ Updates the audio input volume for multiple microphones.
  * @ volume_index0: input gain index 0
  * @ volume_index1: input gain index 1
  * @ gain_select  : select which pair of gain to be setting
  */
void hal_audio_set_stream_in_volume_for_multiple_microphone(uint16_t volume_index0, uint16_t volume_index1, hal_audio_input_gain_select_t gain_select)
{
    hal_audio_volume_analog_input_gain_parameter_t      analog_input_gain;
    uint32_t gain_id0, gain_id1;
    bool is_analog_gain = false;
    memset(&analog_input_gain,0,sizeof(hal_audio_volume_analog_input_gain_parameter_t));
    analog_input_gain.value_l = HAL_AUDIO_INVALID_DIGITAL_GAIN_INDEX;
    analog_input_gain.value_r = HAL_AUDIO_INVALID_DIGITAL_GAIN_INDEX;
    gain_id0 = INPUT_DIGITAL_GAIN_NUM;
    gain_id1 = INPUT_DIGITAL_GAIN_NUM;
    switch (gain_select) {
        case HAL_AUDIO_INPUT_GAIN_SELECTION_D0_D1:
            gain_id0 = INPUT_DIGITAL_GAIN_FOR_MIC0_L;
            gain_id1 = INPUT_DIGITAL_GAIN_FOR_MIC0_R;
            break;
        case HAL_AUDIO_INPUT_GAIN_SELECTION_D2_D3:
            gain_id0 = INPUT_DIGITAL_GAIN_FOR_MIC1_L;
            gain_id1 = INPUT_DIGITAL_GAIN_FOR_MIC1_R;
            break;
        case HAL_AUDIO_INPUT_GAIN_SELECTION_D4_D5:
            gain_id0 = INPUT_DIGITAL_GAIN_FOR_MIC2_L;
            gain_id1 = INPUT_DIGITAL_GAIN_FOR_MIC2_R;
            break;
        case HAL_AUDIO_INPUT_GAIN_SELECTION_D6_D7:
            gain_id0 = INPUT_DIGITAL_GAIN_FOR_I2S0_L;
            gain_id1 = INPUT_DIGITAL_GAIN_FOR_I2S0_R;
            break;
        case HAL_AUDIO_INPUT_GAIN_SELECTION_D8_D9:
            gain_id0 = INPUT_DIGITAL_GAIN_FOR_I2S1_L;
            gain_id1 = INPUT_DIGITAL_GAIN_FOR_I2S1_R;
            break;
        case HAL_AUDIO_INPUT_GAIN_SELECTION_D10_D11:
            gain_id0 = INPUT_DIGITAL_GAIN_FOR_I2S2_L;
            gain_id1 = INPUT_DIGITAL_GAIN_FOR_I2S2_R;
            break;
        case HAL_AUDIO_INPUT_GAIN_SELECTION_D12_D13:
            gain_id0 = INPUT_DIGITAL_GAIN_FOR_LINEIN_L;
            gain_id1 = INPUT_DIGITAL_GAIN_FOR_LINEIN_R;
            break;
        case HAL_AUDIO_INPUT_GAIN_SELECTION_D14:
            gain_id0 = INPUT_DIGITAL_GAIN_FOR_ECHO_PATH;
            break;
#ifdef MTK_SPECIAL_FUNCTIONS_ENABLE
        case HAL_AUDIO_INPUT_GAIN_SELECTION_D18_19:
            gain_id0 = INPUT_DIGITAL_GAIN_FOR_SPECIAL_FUNCTION_MIC0_L;
            gain_id1 = INPUT_DIGITAL_GAIN_FOR_SPECIAL_FUNCTION_MIC0_R;
            break;
        case HAL_AUDIO_INPUT_GAIN_SELECTION_D20_21:
            gain_id0 = INPUT_DIGITAL_GAIN_FOR_SPECIAL_FUNCTION_MIC1_L;
            gain_id1 = INPUT_DIGITAL_GAIN_FOR_SPECIAL_FUNCTION_MIC1_R;
            break;
        case HAL_AUDIO_INPUT_GAIN_SELECTION_D22_23:
            gain_id0 = INPUT_DIGITAL_GAIN_FOR_SPECIAL_FUNCTION_MIC2_L;
            gain_id1 = INPUT_DIGITAL_GAIN_FOR_SPECIAL_FUNCTION_MIC2_R;
            break;
        case HAL_AUDIO_INPUT_GAIN_SELECTION_D24_25:
            gain_id0 = INPUT_DIGITAL_GAIN_FOR_SPECIAL_FUNCTION_ECHO;
            break;
#endif
        case HAL_AUDIO_INPUT_GAIN_SELECTION_A0_A1:
            gain_id0 = INPUT_ANALOG_GAIN_FOR_MIC0_L;
            gain_id1 = INPUT_ANALOG_GAIN_FOR_MIC0_R;
            is_analog_gain = true;
            analog_input_gain.device_interface = HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;
            break;
        case HAL_AUDIO_INPUT_GAIN_SELECTION_A2_A3:
            gain_id0 = INPUT_ANALOG_GAIN_FOR_MIC1_L;
            gain_id1 = INPUT_ANALOG_GAIN_FOR_MIC1_R;
            is_analog_gain = true;
            analog_input_gain.device_interface = HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2;
            break;
        case HAL_AUDIO_INPUT_GAIN_SELECTION_A4_A5:
            gain_id0 = INPUT_ANALOG_GAIN_FOR_MIC2_L;
            gain_id1 = INPUT_ANALOG_GAIN_FOR_MIC2_R;
            is_analog_gain = true;
            analog_input_gain.device_interface = HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3;
            break;
        default:
            HAL_AUDIO_LOG_INFO("DSP in_volume_for_multiple_microphone unsupport gain :%d", 1, gain_select);
            break;
    }
    if (is_analog_gain) {
        if ((volume_index0 != HAL_AUDIO_INVALID_ANALOG_GAIN_INDEX) && (gain_id0!=INPUT_DIGITAL_GAIN_NUM)) {
            afe.stream_in.analog_gain_index[gain_id0] = (uint32_t)((int32_t)((int16_t)volume_index0));
            analog_input_gain.value_l = afe.stream_in.analog_gain_index[gain_id0];
        }
        if ((volume_index1 != HAL_AUDIO_INVALID_ANALOG_GAIN_INDEX) && (gain_id1!=INPUT_DIGITAL_GAIN_NUM)){
            afe.stream_in.analog_gain_index[gain_id1] = (uint32_t)((int32_t)((int16_t)volume_index1));
            analog_input_gain.value_r = afe.stream_in.analog_gain_index[gain_id1];
        }
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&analog_input_gain, HAL_AUDIO_SET_VOLUME_INPUT_ANALOG_GAIN);
    } else {
        if ((volume_index0 != HAL_AUDIO_INVALID_DIGITAL_GAIN_INDEX) && (gain_id0!=INPUT_DIGITAL_GAIN_NUM)) {
            afe.stream_in.digital_gain_index[gain_id0] = (int32_t)((int16_t)volume_index0);
        }
        if ((volume_index1 != HAL_AUDIO_INVALID_DIGITAL_GAIN_INDEX) && (gain_id1!=INPUT_DIGITAL_GAIN_NUM)) {
            afe.stream_in.digital_gain_index[gain_id1] = (int32_t)((int16_t)volume_index1);
        }
    }
    #if 0
    afe_audio_set_input_analog_gain();
    //set input digital gain (use IP's gain)
    #endif

}


/**
  * @ Updates the audio input volume
  * @ digital_volume_index: digital gain index
  * @ analog_volume_index : analog gain index
  */
void hal_audio_set_stream_in_volume(uint16_t digital_volume_index, uint16_t analog_volume_index)
{
    if (analog_volume_index != HAL_AUDIO_INVALID_ANALOG_GAIN_INDEX) {
        afe.stream_in.analog_gain_index[INPUT_ANALOG_GAIN_FOR_MIC0_L]  = (int32_t)((int16_t)analog_volume_index);
        afe.stream_in.analog_gain_index[INPUT_ANALOG_GAIN_FOR_MIC0_R]  = (int32_t)((int16_t)analog_volume_index);
        afe.stream_in.analog_gain_index[INPUT_ANALOG_GAIN_FOR_MIC1_L]  = (int32_t)((int16_t)analog_volume_index);
        afe.stream_in.analog_gain_index[INPUT_ANALOG_GAIN_FOR_MIC1_R]  = (int32_t)((int16_t)analog_volume_index);
        afe.stream_in.analog_gain_index[INPUT_ANALOG_GAIN_FOR_MIC2_L]  = (int32_t)((int16_t)analog_volume_index);
        afe.stream_in.analog_gain_index[INPUT_ANALOG_GAIN_FOR_MIC2_R]  = (int32_t)((int16_t)analog_volume_index);
        hal_audio_volume_analog_input_gain_parameter_t      analog_input_gain;
        memset(&analog_input_gain,0,sizeof(hal_audio_volume_analog_input_gain_parameter_t));
        analog_input_gain.device_interface = HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;
        analog_input_gain.value_l = analog_volume_index;
        analog_input_gain.value_r = analog_volume_index;
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&analog_input_gain, HAL_AUDIO_SET_VOLUME_INPUT_ANALOG_GAIN);
        analog_input_gain.device_interface = HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2;
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&analog_input_gain, HAL_AUDIO_SET_VOLUME_INPUT_ANALOG_GAIN);
        analog_input_gain.device_interface = HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3;
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&analog_input_gain, HAL_AUDIO_SET_VOLUME_INPUT_ANALOG_GAIN);
    }
    if (digital_volume_index != HAL_AUDIO_INVALID_DIGITAL_GAIN_INDEX) {
        afe.stream_in.digital_gain_index[INPUT_DIGITAL_GAIN_FOR_MIC0_L] = (int32_t)((int16_t)digital_volume_index);
        afe.stream_in.digital_gain_index[INPUT_DIGITAL_GAIN_FOR_MIC0_R] = (int32_t)((int16_t)digital_volume_index);
        afe.stream_in.digital_gain_index[INPUT_DIGITAL_GAIN_FOR_MIC1_L] = (int32_t)((int16_t)digital_volume_index);
        afe.stream_in.digital_gain_index[INPUT_DIGITAL_GAIN_FOR_MIC1_R] = (int32_t)((int16_t)digital_volume_index);
        afe.stream_in.digital_gain_index[INPUT_DIGITAL_GAIN_FOR_MIC2_L] = (int32_t)((int16_t)digital_volume_index);
        afe.stream_in.digital_gain_index[INPUT_DIGITAL_GAIN_FOR_MIC2_R] = (int32_t)((int16_t)digital_volume_index);
        afe.stream_in.digital_gain_index[INPUT_DIGITAL_GAIN_FOR_I2S0_L] = (int32_t)((int16_t)digital_volume_index);
        afe.stream_in.digital_gain_index[INPUT_DIGITAL_GAIN_FOR_I2S0_R] = (int32_t)((int16_t)digital_volume_index);
        afe.stream_in.digital_gain_index[INPUT_DIGITAL_GAIN_FOR_I2S1_L] = (int32_t)((int16_t)digital_volume_index);
        afe.stream_in.digital_gain_index[INPUT_DIGITAL_GAIN_FOR_I2S1_R] = (int32_t)((int16_t)digital_volume_index);
        afe.stream_in.digital_gain_index[INPUT_DIGITAL_GAIN_FOR_I2S2_L] = (int32_t)((int16_t)digital_volume_index);
        afe.stream_in.digital_gain_index[INPUT_DIGITAL_GAIN_FOR_I2S2_R] = (int32_t)((int16_t)digital_volume_index);
        afe.stream_in.digital_gain_index[INPUT_DIGITAL_GAIN_FOR_LINEIN_L] = (int32_t)((int16_t)digital_volume_index);
        afe.stream_in.digital_gain_index[INPUT_DIGITAL_GAIN_FOR_LINEIN_R] = (int32_t)((int16_t)digital_volume_index);
        afe.stream_in.digital_gain_index[INPUT_DIGITAL_GAIN_FOR_ECHO_PATH] = (int32_t)((int16_t)digital_volume_index);
    }
    #if 0//modify for ab1568
    afe_audio_set_input_analog_gain();
    //set input digital gain (use IP's gain)
    #else

    #endif
}

#if defined(HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT)
/**
  * @ Mute/Unmute stream ouput path
  * @ mute: true-> set mute / false->set unmute
  * @ hw_gain_index: HAL_AUDIO_STREAM_OUT1-> indicate hw gain1 / HAL_AUDIO_STREAM_OUT2-> indicate hw gain2 / HAL_AUDIO_STREAM_OUT_ALL-> indicate hw gain1 and hw gain2
  */
void hal_audio_mute_stream_out(bool mute, hal_audio_hw_stream_out_index_t hw_gain_index)
{
    #if 0//modify for ab1568
    if (hw_gain_index == HAL_AUDIO_STREAM_OUT1 || hw_gain_index == HAL_AUDIO_STREAM_OUT_ALL) {
        afe.stream_out.mute_flag = mute;
        afe_audio_set_output_digital_gain(AUDIO_HW_GAIN);
    }
    if (hw_gain_index == HAL_AUDIO_STREAM_OUT2 || hw_gain_index == HAL_AUDIO_STREAM_OUT_ALL) {
        afe.stream_out.mute_flag2 = mute;
        afe_audio_set_output_digital_gain(AUDIO_HW_GAIN2);
    }
    #else
    if (hw_gain_index == HAL_AUDIO_STREAM_OUT1) {
        hal_audio_volume_digital_gain_parameter_t digital_gain;
        memset(&digital_gain,0,sizeof(hal_audio_volume_digital_gain_parameter_t));
        digital_gain.memory_select = HAL_AUDIO_MEMORY_DL_DL1;
        digital_gain.mute_control = HAL_AUDIO_VOLUME_MUTE_ZERO_PADDING;
        digital_gain.mute_enable = mute;
        digital_gain.is_mute_control = true;
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&digital_gain, HAL_AUDIO_SET_VOLUME_HW_DIGITAL_GAIN);

    }
    if (hw_gain_index == HAL_AUDIO_STREAM_OUT2) {
        hal_audio_volume_digital_gain_parameter_t digital_gain;
        memset(&digital_gain,0,sizeof(hal_audio_volume_digital_gain_parameter_t));
        digital_gain.memory_select = HAL_AUDIO_MEMORY_DL_DL2;
        digital_gain.mute_control = HAL_AUDIO_VOLUME_MUTE_ZERO_PADDING;
        digital_gain.mute_enable = mute;
        digital_gain.is_mute_control = true;
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&digital_gain, HAL_AUDIO_SET_VOLUME_HW_DIGITAL_GAIN);

    }
    if(hw_gain_index == HAL_AUDIO_STREAM_OUT3) {//for DL3
        hal_audio_volume_digital_gain_parameter_t digital_gain;
        memset(&digital_gain,0,sizeof(hal_audio_volume_digital_gain_parameter_t));
        digital_gain.memory_select = HAL_AUDIO_MEMORY_DL_DL3;
        digital_gain.mute_control = HAL_AUDIO_VOLUME_MUTE_ZERO_PADDING;
        digital_gain.mute_enable = mute;
        digital_gain.is_mute_control = true;
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&digital_gain, HAL_AUDIO_SET_VOLUME_HW_DIGITAL_GAIN);
    }
    if(hw_gain_index == HAL_AUDIO_STREAM_OUT_ALL){
        hal_audio_volume_digital_gain_parameter_t digital_gain;
        memset(&digital_gain,0,sizeof(hal_audio_volume_digital_gain_parameter_t));
        digital_gain.memory_select = HAL_AUDIO_MEMORY_DL_DL1|HAL_AUDIO_MEMORY_DL_DL2|HAL_AUDIO_MEMORY_DL_DL3;
        digital_gain.mute_control = HAL_AUDIO_VOLUME_MUTE_ZERO_PADDING;
        digital_gain.mute_enable = mute;
        digital_gain.is_mute_control = true;
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&digital_gain, HAL_AUDIO_SET_VOLUME_HW_DIGITAL_GAIN);
    }
    #endif

}
#else
/**
  * @ Mute stream ouput path
  * @ mute: true-> set mute / false->set unmute
  */
void hal_audio_mute_stream_out(bool mute)
{
    #if 0
    afe.stream_out.mute_flag = mute;
    afe_audio_set_output_analog_gain();
    #else
    hal_audio_volume_digital_gain_parameter_t           digital_gain;
    memset(&digital_gain,0,sizeof(hal_audio_volume_digital_gain_parameter_t));
    digital_gain.memory_select = HAL_AUDIO_MEMORY_DL_DL1|HAL_AUDIO_MEMORY_DL_DL2;
    digital_gain.mute_control = HAL_AUDIO_VOLUME_MUTE_ZERO_PADDING;
    digital_gain.mute_enable = mute;
    digital_gain.is_mute_control = true;
    hal_audio_set_value(&digital_gain, HAL_AUDIO_SET_VOLUME_HW_DIGITAL_GAIN);

    #endif
}
#endif


/**
  * @ Mute stream input path
  * @ mute: true-> set mute / false->set unmute
  */
void hal_audio_mute_stream_in(bool mute)
{
    UNUSED(mute);
    #if 0//modify for ab1568
    afe.stream_in.mute_flag = mute;
    afe_audio_set_input_analog_gain();
    #else

    #endif
}

/**
  * @ Updates the audio output channel number
  * @ channel_number : audio channel mode to play the audio stream
  * @ This API should be called before hal_audio_start_stream_out() to adjust the output channel number
  * @ Retval: HAL_AUDIO_STATUS_OK if operation success, others if channel number is invalid
  */
hal_audio_status_t hal_audio_set_stream_out_channel_number(hal_audio_channel_number_t channel_number)
{
    hal_audio_status_t result = HAL_AUDIO_STATUS_OK;
    switch (channel_number) {
        case HAL_AUDIO_MONO:
            afe.stream_out.stream_channel = HAL_AUDIO_MONO;
            break;
        case HAL_AUDIO_STEREO:
        case HAL_AUDIO_STEREO_BOTH_L_CHANNEL:
        case HAL_AUDIO_STEREO_BOTH_R_CHANNEL:
        case HAL_AUDIO_STEREO_BOTH_L_R_SWAP:
            afe.stream_out.stream_channel = HAL_AUDIO_STEREO;
            break;
        default:
            result = HAL_AUDIO_STATUS_INVALID_PARAMETER;
            break;
    }
    return result;
}

/**
  * @ Updates the audio input channel number
  * @ channel_number : audio channel mode to record the audio stream
  * @ This API should be called before hal_audio_start_stream_in() to adjust the input channel number
  * @ Retval: HAL_AUDIO_STATUS_OK if operation is successful, others if channel number is invalid
  */
hal_audio_status_t hal_audio_set_stream_in_channel_number(hal_audio_channel_number_t channel_number)
{
    switch (channel_number) {
        case HAL_AUDIO_MONO:
        case HAL_AUDIO_STEREO:
            afe.stream_in.stream_channel = channel_number;
            return HAL_AUDIO_STATUS_OK;
        default:
            return HAL_AUDIO_STATUS_INVALID_PARAMETER;
    }
}

/**
  * @ Query the output channel number
  */
hal_audio_channel_number_t hal_audio_get_stream_out_channel_number(void)
{
    return afe.stream_out.stream_channel;
}


void hal_audio_afe_register_irq_callback(hal_audio_irq_callback_function_t* function)
{
    afe.func_handle = function;
}

void hal_audio_afe_register_amp_handle(hal_amp_function_t *function)
{
#ifdef ENABLE_AMP_TIMER
    afe.amp_handle = function;
#else
    UNUSED(function);
#endif
}
#define CONN_BT_TIMCON_BASE 0xB0000000

/*
    nat_clk   => audio_play_en trigger native clock
    intra_clk => audio_play_en trigger native intra clock
    bit         => 0x0 : disable, 0x1 : audio play en, 0x2 audio get cnt
*/
void hal_audio_afe_set_play_en(U32 nat_clk, U32 intra_clk)
{
    *((volatile uint32_t *)(CONN_BT_TIMCON_BASE + 0x0204)) = nat_clk;
    *((volatile uint32_t *)(CONN_BT_TIMCON_BASE + 0x0208)) = intra_clk;
    *((volatile uint8_t *)(CONN_BT_TIMCON_BASE + 0x0200)) = 0xFF;

    AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0x100,    0);        //xtal_cnt powered on
    AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0x100,    0x100);
    AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0x1000,   0x1000);   //reset the xtal_cnt to zero
    AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0x1000,   0);
}
void hal_audio_afe_disable_play_en(void)
{
    *((volatile uint8_t *)(CONN_BT_TIMCON_BASE + 0x0200)) = 0;
}


#endif /*HAL_AUDIO_MODULE_ENABLED*/
