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


/* Includes ------------------------------------------------------------------*/

#include "bt_sink_srv_ami.h"
#include "linein_playback.h"
#include "hal_audio_message_struct.h"
#include "hal_audio_internal.h"
#include "hal_audio_cm4_dsp_message.h"
#include "syslog.h"
#include "hal_audio.h"
//#include "hal_clock_platform.h"

#if defined(MTK_AVM_DIRECT)
extern HAL_DSP_PARA_AU_AFE_CTRL_t audio_nvdm_HW_config;
#endif

log_create_module(linein, PRINT_LEVEL_INFO);
#define LINEIN_PLAYBACK_USE_MSGID_LOG
#ifdef LINEIN_PLAYBACK_USE_MSGID_LOG
#define LINEIN_PLAYBACK_LOG_E(fmt,arg...)   LOG_MSGID_E(linein, "Linein : "fmt,##arg)
#define LINEIN_PLAYBACK_LOG_W(fmt,arg...)   LOG_MSGID_W(linein, "Linein : "fmt,##arg)
#define LINEIN_PLAYBACK_LOG_I(fmt,arg...)   LOG_MSGID_I(linein ,"Linein : "fmt,##arg)
#else
#define LINEIN_PLAYBACK_LOG_E(fmt,arg...)   LOG_E(linein, "Linein : "fmt,##arg)
#define LINEIN_PLAYBACK_LOG_W(fmt,arg...)   LOG_W(linein, "Linein : "fmt,##arg)
#define LINEIN_PLAYBACK_LOG_I(fmt,arg...)   LOG_I(linein ,"Linein : "fmt,##arg)
#endif

static linein_playback_state_t linein_status;

#if 0
static uint32_t sampling_rate_enum_to_value(hal_audio_sampling_rate_t hal_audio_sampling_rate_enum)
{
    switch (hal_audio_sampling_rate_enum) {
        case HAL_AUDIO_SAMPLING_RATE_8KHZ:
            return   8000;
        case HAL_AUDIO_SAMPLING_RATE_11_025KHZ:
            return  11025;
        case HAL_AUDIO_SAMPLING_RATE_12KHZ:
            return  12000;
        case HAL_AUDIO_SAMPLING_RATE_16KHZ:
            return  16000;
        case HAL_AUDIO_SAMPLING_RATE_22_05KHZ:
            return  22050;
        case HAL_AUDIO_SAMPLING_RATE_24KHZ:
            return  24000;
        case HAL_AUDIO_SAMPLING_RATE_32KHZ:
            return  32000;
        case HAL_AUDIO_SAMPLING_RATE_44_1KHZ:
            return  44100;
        case HAL_AUDIO_SAMPLING_RATE_48KHZ:
            return  48000;
        case HAL_AUDIO_SAMPLING_RATE_88_2KHZ:
            return  88200;
        case HAL_AUDIO_SAMPLING_RATE_96KHZ:
            return  96000;
        case HAL_AUDIO_SAMPLING_RATE_176_4KHZ:
            return 176400;
        case HAL_AUDIO_SAMPLING_RATE_192KHZ:
            return 192000;

        default:
            return 8000;
    }
}
#endif

#ifdef AIR_LE_AUDIO_ENABLE
linein_result_t linein_playback_le_audio_open(hal_audio_sampling_rate_t linein_sample_rate, hal_audio_sampling_rate_t dacout_sample_rate, uint16_t frame_sample_count)
{
    hal_audio_device_t in_audio_device  = HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL;
    hal_audio_device_t out_audio_device = HAL_AUDIO_DEVICE_DAC_DUAL;

#if defined(MTK_AVM_DIRECT)
    if(!hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_LINEIN)){
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_LINEIN, true);
    }
#endif
    LINEIN_PLAYBACK_LOG_I("enter linein_playback_le_audio_open, in rate=%d, out rate=%d, in device=0x%x, out device=0x%x, sample count=%d\n",
            5, hal_audio_sampling_rate_enum_to_value(linein_sample_rate),hal_audio_sampling_rate_enum_to_value(dacout_sample_rate), in_audio_device, out_audio_device, frame_sample_count);

    if (linein_status != LINEIN_STATE_IDLE) {
        LINEIN_PLAYBACK_LOG_I("cannot open because state(%d)\n", 1, linein_status);
        return LINEIN_EXECUTION_FAIL;
    }

//    if (linein_sample_rate != HAL_AUDIO_SAMPLING_RATE_48KHZ && linein_sample_rate != HAL_AUDIO_SAMPLING_RATE_96KHZ && linein_sample_rate != HAL_AUDIO_SAMPLING_RATE_192KHZ) {
//        LINEIN_PLAYBACK_LOG_I("not support sample rate=%d\n", 1, hal_audio_sampling_rate_enum_to_value(linein_sample_rate));
//        return LINEIN_EXECUTION_FAIL;
//    }

    mcu2dsp_open_param_t open_param = {{0}};
    void *p_param_share;

    open_param.param.stream_in = STREAM_IN_AFE;
    open_param.param.stream_out = STREAM_OUT_AFE;


//#if defined (HAL_DVFS_MODULE_ENABLED)
//    hal_dvfs_lock_control(HAL_DVFS_MAX_SPEED, HAL_DVFS_LOCK);
//#endif

    open_param.stream_in_param.afe.audio_device = in_audio_device;
    open_param.stream_in_param.afe.stream_channel = HAL_AUDIO_DIRECT;
    open_param.stream_in_param.afe.memory = HAL_AUDIO_MEM1 ;
    open_param.stream_in_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
    open_param.stream_in_param.afe.format = AFE_PCM_FORMAT_S16_LE;
    open_param.stream_in_param.afe.sampling_rate = hal_audio_sampling_rate_enum_to_value(linein_sample_rate);
    open_param.stream_in_param.afe.irq_period = 0;
    open_param.stream_in_param.afe.frame_size = frame_sample_count;
    open_param.stream_in_param.afe.frame_number = 4;
    open_param.stream_in_param.afe.hw_gain = false;
    open_param.stream_in_param.afe.adc_mode = HAL_AUDIO_ANALOG_INPUT_ACC10K;
    open_param.stream_in_param.afe.misc_parms      = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_2p4v;
    open_param.stream_in_param.afe.ul_adc_mode[0] = 0;// ACC_10k

    open_param.stream_out_param.afe.audio_device = out_audio_device;
    open_param.stream_out_param.afe.stream_channel = HAL_AUDIO_DIRECT;
    open_param.stream_out_param.afe.memory = HAL_AUDIO_MEM1;
    open_param.stream_out_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
    open_param.stream_out_param.afe.format = AFE_PCM_FORMAT_S16_LE;
    open_param.stream_out_param.afe.stream_out_sampling_rate = hal_audio_sampling_rate_enum_to_value(dacout_sample_rate);
#if defined (FIXED_SAMPLING_RATE_TO_48KHZ)
    open_param.stream_out_param.afe.sampling_rate   = HAL_AUDIO_FIXED_AFE_48K_SAMPLE_RATE;
#elif defined (AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ)
    open_param.stream_out_param.afe.sampling_rate   = HAL_AUDIO_FIXED_AFE_96K_SAMPLE_RATE;
#else
    open_param.stream_out_param.afe.sampling_rate = hal_audio_sampling_rate_enum_to_value(dacout_sample_rate);
#endif
    open_param.stream_out_param.afe.irq_period = 0;
    open_param.stream_out_param.afe.frame_size = frame_sample_count;
    open_param.stream_out_param.afe.frame_number = 4;
    open_param.stream_out_param.afe.hw_gain = true;
    open_param.stream_out_param.afe.adc_mode = HAL_AUDIO_ANALOG_OUTPUT_CLASSAB;
    open_param.stream_out_param.afe.misc_parms      = DOWNLINK_PERFORMANCE_NORMAL;
    open_param.stream_out_param.afe.dl_dac_mode = 0;//Class_G

#ifdef ENABLE_2A2D_TEST
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    open_param.stream_in_param.afe.audio_device1 = in_audio_device;
    open_param.stream_in_param.afe.audio_interface1 = HAL_AUDIO_INTERFACE_1;
    open_param.stream_out_param.afe.audio_device1 = out_audio_device;
    open_param.stream_out_param.afe.audio_interface1 = HAL_AUDIO_INTERFACE_1;
#endif
#endif


#ifdef ENABLE_HWSRC_CLKSKEW
    open_param.stream_in_param.afe.clkskew_mode  = CLK_SKEW_V1;
    open_param.stream_out_param.afe.clkskew_mode  = CLK_SKEW_V1;
#endif

    // Open uplink high res
    hal_audio_dsp_ul_clkmux_control(in_audio_device, hal_audio_sampling_rate_enum_to_value(linein_sample_rate), true);
    // Open downlink high res
    hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_LINEIN, out_audio_device, open_param.stream_out_param.afe.sampling_rate, true);

#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
    ami_set_afe_param(STREAM_OUT, linein_sample_rate, true);
#endif
    p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), AUDIO_MESSAGE_TYPE_LINEIN);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_LINEIN_PLAYBACK_OPEN, 0, (uint32_t)p_param_share, true);

    hal_audio_set_stream_in_volume(0,0);
    hal_audio_set_stream_out_volume(0,0);

    linein_status = LINEIN_STATE_READY;
    return LINEIN_EXECUTION_SUCCESS;
}
#endif

linein_result_t linein_playback_open(hal_audio_sampling_rate_t linein_sample_rate, hal_audio_device_t in_audio_device, hal_audio_device_t out_audio_device)
{
#if defined(MTK_AVM_DIRECT)
    if(!hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_LINEIN)){
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_LINEIN, true);
    }
#endif
    LINEIN_PLAYBACK_LOG_I("enter linein_playback_open, sample_rate=%d, in_audio_device=0x%x, out_audio_device=0x%x\n", 3, hal_audio_sampling_rate_enum_to_value(linein_sample_rate), in_audio_device, out_audio_device);
    if (linein_status != LINEIN_STATE_IDLE) {
        LINEIN_PLAYBACK_LOG_I("cannot open because state(%d)\n", 1, linein_status);
        return LINEIN_EXECUTION_FAIL;
    }

    if (linein_sample_rate != HAL_AUDIO_SAMPLING_RATE_48KHZ && linein_sample_rate != HAL_AUDIO_SAMPLING_RATE_96KHZ && linein_sample_rate != HAL_AUDIO_SAMPLING_RATE_192KHZ) {
        LINEIN_PLAYBACK_LOG_I("not support sample rate=%d\n", 1, hal_audio_sampling_rate_enum_to_value(linein_sample_rate));
        return LINEIN_EXECUTION_FAIL;
    }
    if (in_audio_device != HAL_AUDIO_DEVICE_I2S_MASTER && in_audio_device != HAL_AUDIO_DEVICE_I2S_SLAVE && ((in_audio_device & HAL_AUDIO_DEVICE_MAIN_MIC_DUAL) == false)
        && ((in_audio_device & HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL) == false) && ((in_audio_device & HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL) == false)) {
        LINEIN_PLAYBACK_LOG_I("not support in device=%d\n", 1, in_audio_device);
        return LINEIN_EXECUTION_FAIL;
    }
    if (out_audio_device != HAL_AUDIO_DEVICE_I2S_MASTER && out_audio_device != HAL_AUDIO_DEVICE_I2S_SLAVE && ((out_audio_device & HAL_AUDIO_DEVICE_DAC_DUAL) == false)) {
        LINEIN_PLAYBACK_LOG_I("not support out device=%d\n", 1, out_audio_device);
        return LINEIN_EXECUTION_FAIL;
    }

    mcu2dsp_open_param_t open_param;
    memset(&open_param, 0, sizeof(mcu2dsp_open_param_t));
    void *p_param_share;

    open_param.param.stream_in = STREAM_IN_AFE;
    open_param.param.stream_out = STREAM_OUT_AFE;

    open_param.stream_in_param.afe.audio_device = in_audio_device;
#ifdef ENABLE_2A2D_TEST
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    open_param.stream_in_param.afe.audio_device1 = in_audio_device;
#endif
#endif
    open_param.stream_in_param.afe.stream_channel = HAL_AUDIO_DIRECT;
    open_param.stream_in_param.afe.memory = HAL_AUDIO_MEM1 ;
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    open_param.stream_in_param.afe.audio_interface = hal_audio_convert_linein_interface(audio_nvdm_HW_config.audio_scenario.Audio_Linein_Input_Path,true);
#else
    open_param.stream_in_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
#endif

#ifdef ENABLE_2A2D_TEST
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    open_param.stream_in_param.afe.audio_interface1 = hal_audio_convert_linein_interface(audio_nvdm_HW_config.audio_scenario.Audio_Linein_Input_Path,true);
#endif
#endif
    open_param.stream_in_param.afe.format = AFE_PCM_FORMAT_S32_LE;
    open_param.stream_in_param.afe.sampling_rate = hal_audio_sampling_rate_enum_to_value(linein_sample_rate);
    //open_param.stream_in_param.afe.irq_period = 512 / (hal_audio_sampling_rate_enum_to_value(linein_sample_rate)/1000);
    open_param.stream_in_param.afe.irq_period = 0;
    open_param.stream_in_param.afe.frame_size = 512;
    open_param.stream_in_param.afe.frame_number = 4;
    open_param.stream_in_param.afe.hw_gain = false;

#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    //Linein parameters
    open_param.stream_in_param.afe.performance = audio_nvdm_HW_config.audio_scenario.Audio_Analog_LineIn_Performance_Sel;
    open_param.stream_in_param.afe.iir_filter[0] = audio_nvdm_HW_config.adc_dac_config.ADDA_Audio_IIR_Filter;
    open_param.stream_in_param.afe.ul_adc_mode[0] = audio_nvdm_HW_config.adc_dac_config.ADDA_Analog_MIC0_Mode;

    //I2S Slave parameters
    open_param.stream_in_param.afe.i2s_format = audio_nvdm_HW_config.I2SS_config.I2S_Slave_Format;
    open_param.stream_in_param.afe.i2s_word_length = audio_nvdm_HW_config.I2SS_config.I2S_Slave_Word_length;
#endif

    if(open_param.stream_in_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
        open_param.stream_in_param.afe.misc_parms      = I2S_CLK_SOURCE_DCXO;
        if (linein_sample_rate > HAL_AUDIO_SAMPLING_RATE_48KHZ) {
            open_param.stream_in_param.afe.misc_parms  = I2S_CLK_SOURCE_APLL;
        }
    } else {
        if (in_audio_device & HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL) {
            open_param.stream_in_param.afe.misc_parms      = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_2p4v;
        } else {
            open_param.stream_in_param.afe.misc_parms      = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_1p85v;
        }
    }

    open_param.stream_out_param.afe.audio_device = out_audio_device;
    open_param.stream_out_param.afe.stream_channel = HAL_AUDIO_DIRECT;
    open_param.stream_out_param.afe.memory = HAL_AUDIO_MEM1;
    open_param.stream_out_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
    open_param.stream_out_param.afe.format = AFE_PCM_FORMAT_S32_LE;
    open_param.stream_out_param.afe.stream_out_sampling_rate = hal_audio_sampling_rate_enum_to_value(linein_sample_rate);
#if defined (FIXED_SAMPLING_RATE_TO_48KHZ)
    open_param.stream_out_param.afe.sampling_rate   = HAL_AUDIO_FIXED_AFE_48K_SAMPLE_RATE;
#elif defined (AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ)
    open_param.stream_out_param.afe.sampling_rate   = HAL_AUDIO_FIXED_AFE_96K_SAMPLE_RATE;
#else
    open_param.stream_out_param.afe.sampling_rate   = hal_audio_sampling_rate_enum_to_value(linein_sample_rate);
#endif
    //open_param.stream_out_param.afe.irq_period = 512 / (hal_audio_sampling_rate_enum_to_value(linein_sample_rate)/1000);
    open_param.stream_out_param.afe.irq_period = 0;
    open_param.stream_out_param.afe.frame_size = 512;
    open_param.stream_out_param.afe.frame_number = 4;
    open_param.stream_out_param.afe.hw_gain = true;
#ifdef ENABLE_HWSRC_CLKSKEW
    open_param.stream_out_param.afe.clkskew_mode = CLK_SKEW_V1;
#endif

#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    open_param.stream_out_param.afe.performance = audio_nvdm_HW_config.adc_dac_config.ADDA_DAC_Class_AB_G_Performance_Sel;
    switch(audio_nvdm_HW_config.adc_dac_config.ADDA_DAC_Mode_Sel) {
        case 0x00: //Class G
            open_param.stream_out_param.afe.dl_dac_mode = HAL_AUDIO_ANALOG_OUTPUT_CLASSG;
            break;
        case 0x01: //Class AB
            open_param.stream_out_param.afe.dl_dac_mode = HAL_AUDIO_ANALOG_OUTPUT_CLASSAB;
            break;
        case 0x02: //Class D
            open_param.stream_out_param.afe.dl_dac_mode = HAL_AUDIO_ANALOG_OUTPUT_CLASSD;
            break;
        default:
            open_param.stream_out_param.afe.dl_dac_mode = HAL_AUDIO_ANALOG_OUTPUT_CLASSAB;
            break;
    }

    //I2S Slave parameters
    open_param.stream_out_param.afe.i2s_format = audio_nvdm_HW_config.I2SS_config.I2S_Slave_Format;
    open_param.stream_out_param.afe.i2s_word_length = audio_nvdm_HW_config.I2SS_config.I2S_Slave_Word_length;
#endif
    if(open_param.stream_out_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
        open_param.stream_out_param.afe.misc_parms      = I2S_CLK_SOURCE_DCXO;
        if (linein_sample_rate > HAL_AUDIO_SAMPLING_RATE_48KHZ) {
            open_param.stream_out_param.afe.misc_parms  = I2S_CLK_SOURCE_APLL;
        }
    } else {
        open_param.stream_out_param.afe.misc_parms      = DOWNLINK_PERFORMANCE_NORMAL;
    }
#if defined(HAL_AUDIO_SUPPORT_APLL)
    //Open i2s master in low jitter mode
    hal_audio_i2s_master_in_low_jitter_mode(LOW_JITTER_MODE_LINEIN_PLAYBACK_IN, in_audio_device, hal_audio_sampling_rate_enum_to_value(linein_sample_rate), open_param.stream_in_param.afe.is_low_jitter, true);
    //Open i2s master out low jitter mode
    hal_audio_i2s_master_out_low_jitter_mode(LOW_JITTER_MODE_LINEIN_PLAYBACK_OUT, out_audio_device, open_param.stream_out_param.afe.sampling_rate, open_param.stream_out_param.afe.is_low_jitter, true);
#endif

    // Open uplink high res
    hal_audio_dsp_ul_clkmux_control(in_audio_device, hal_audio_sampling_rate_enum_to_value(linein_sample_rate), true);
    // Open downlink high res
    hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_LINEIN, out_audio_device, open_param.stream_out_param.afe.sampling_rate, true);

#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
    ami_set_afe_param(STREAM_OUT, linein_sample_rate, true);
#endif
    p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), AUDIO_MESSAGE_TYPE_LINEIN);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_LINEIN_PLAYBACK_OPEN, 0, (uint32_t)p_param_share, true);

    linein_status = LINEIN_STATE_READY;
    return LINEIN_EXECUTION_SUCCESS;
}

linein_result_t linein_playback_start()
{
    LINEIN_PLAYBACK_LOG_I("enter linein_playback_start\n", 0);
    if (linein_status != LINEIN_STATE_READY) {
        LINEIN_PLAYBACK_LOG_I("cannot start because state(%d)\n", 1, linein_status);
        return LINEIN_EXECUTION_FAIL;
    }

    mcu2dsp_start_param_t start_param = {{0}};
    void *p_param_share;

    // Collect parameters
    start_param.param.stream_in     = STREAM_IN_AFE;
    start_param.param.stream_out    = STREAM_OUT_AFE;
    start_param.stream_in_param.afe.aws_flag   =  false;
    start_param.stream_out_param.afe.aws_flag   =  false;
    p_param_share = hal_audio_dsp_controller_put_paramter( &start_param, sizeof(mcu2dsp_start_param_t), AUDIO_MESSAGE_TYPE_LINEIN);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_LINEIN_PLAYBACK_START, 0, (uint32_t)p_param_share, true);

    linein_status = LINEIN_STATE_PLAY;
    return LINEIN_EXECUTION_SUCCESS;
}

linein_result_t linein_playback_stop()
{
    LINEIN_PLAYBACK_LOG_I("enter linein_playback_stop\n", 0);
    if (linein_status != LINEIN_STATE_PLAY) {
        LINEIN_PLAYBACK_LOG_I("cannot stop because state(%d)\n", 1, linein_status);
        return LINEIN_EXECUTION_FAIL;
    }
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_LINEIN_PLAYBACK_STOP, 0, 0, true);

    linein_status = LINEIN_STATE_STOP;
    return LINEIN_EXECUTION_SUCCESS;
}

linein_result_t linein_playback_close()
{
    LINEIN_PLAYBACK_LOG_I("enter linein_playback_close\n", 0);
    if (linein_status != LINEIN_STATE_STOP) {
        LINEIN_PLAYBACK_LOG_I("cannot close because state(%d)\n", 1, linein_status);
        return LINEIN_EXECUTION_FAIL;
    }
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_LINEIN_PLAYBACK_CLOSE, 0, 0, true);

#if defined(HAL_AUDIO_SUPPORT_APLL)
    //Close i2s master out low jitter mode
    hal_audio_i2s_master_out_low_jitter_mode(LOW_JITTER_MODE_LINEIN_PLAYBACK_OUT, 0, 0, 0, false);
    //Close i2s master in low jitter mode
    hal_audio_i2s_master_in_low_jitter_mode(LOW_JITTER_MODE_LINEIN_PLAYBACK_IN, 0, 0, 0, false);
#endif

    // Close downlink high res
    hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_LINEIN, 0, 0, false);
    // Close uplink high res
    hal_audio_dsp_ul_clkmux_control(0, 0, false);


    linein_status = LINEIN_STATE_IDLE;
#if defined(MTK_AVM_DIRECT)
    if(hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_LINEIN)){
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_LINEIN, false);
    }
#endif
    return LINEIN_EXECUTION_SUCCESS;
}

linein_result_t linein_playback_set_volume(linein_playback_gain_t gain)
{
    LINEIN_PLAYBACK_LOG_I("enter linein_playback_set_volume\n", 0);
    LINEIN_PLAYBACK_LOG_I("input_digital_gain=%d input_analog_gain=%d\n", 2, gain.in_digital_gain, gain.in_analog_gain);
    LINEIN_PLAYBACK_LOG_I("output_digital_gain=%d output_analog_gain=%d\n", 2, gain.out_digital_gain, gain.out_analog_gain);
    hal_audio_set_stream_in_volume(gain.in_digital_gain, gain.in_analog_gain);
    hal_audio_set_stream_out_volume(gain.out_digital_gain, gain.out_analog_gain);

    return LINEIN_EXECUTION_SUCCESS;
}
#ifdef LINE_IN_PURE_FOR_AMIC_CLASS_G_HQA
linein_result_t pure_linein_playback_open_HQA(hal_audio_sampling_rate_t linein_sample_rate, hal_audio_device_t in_audio_device,hal_audio_interface_t device_in_interface_HQA,hal_audio_analog_mdoe_t adc_mode_HQA,hal_audio_performance_mode_t mic_performance_HQA, hal_audio_device_t out_audio_device,hal_audio_performance_mode_t dac_performance_HQA)
{
#if defined(MTK_AVM_DIRECT)
    if(!hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_LINEIN)){
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_LINEIN, true);
    }
#endif
    LINEIN_PLAYBACK_LOG_I("enter pure_linein_playback_open_HQA, sample_rate=%d, in_audio_device=0x%x, out_audio_device=0x%x\n", 3, hal_audio_sampling_rate_enum_to_value(linein_sample_rate), in_audio_device, out_audio_device);
    if (linein_status != LINEIN_STATE_IDLE) {
        LINEIN_PLAYBACK_LOG_I("cannot open because state(%d)\n", 1, linein_status);
        return LINEIN_EXECUTION_FAIL;
    }

    if (linein_sample_rate != HAL_AUDIO_SAMPLING_RATE_16KHZ && linein_sample_rate != HAL_AUDIO_SAMPLING_RATE_48KHZ && linein_sample_rate != HAL_AUDIO_SAMPLING_RATE_96KHZ && linein_sample_rate != HAL_AUDIO_SAMPLING_RATE_192KHZ) {
        LINEIN_PLAYBACK_LOG_I("not support sample rate=%d\n", 1, hal_audio_sampling_rate_enum_to_value(linein_sample_rate));
        return LINEIN_EXECUTION_FAIL;
    }
    if (in_audio_device != HAL_AUDIO_DEVICE_I2S_MASTER && in_audio_device != HAL_AUDIO_DEVICE_I2S_SLAVE && ((in_audio_device & HAL_AUDIO_DEVICE_MAIN_MIC_DUAL) == false)
        && ((in_audio_device & HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL) == false) && ((in_audio_device & HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL) == false)) {
        LINEIN_PLAYBACK_LOG_I("not support in device=%d\n", 1, in_audio_device);
        return LINEIN_EXECUTION_FAIL;
    }
    if (out_audio_device != HAL_AUDIO_DEVICE_I2S_MASTER && out_audio_device != HAL_AUDIO_DEVICE_I2S_SLAVE && ((out_audio_device & HAL_AUDIO_DEVICE_DAC_DUAL) == false)) {
        LINEIN_PLAYBACK_LOG_I("not support out device=%d\n", 1, out_audio_device);
        return LINEIN_EXECUTION_FAIL;
    }

    mcu2dsp_open_param_t open_param;
    void *p_param_share;

    open_param.param.stream_in = STREAM_IN_AFE;
    open_param.param.stream_out = STREAM_OUT_AFE;

    open_param.stream_in_param.afe.audio_interface = device_in_interface_HQA;

#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    open_param.stream_in_param.afe.performance = mic_performance_HQA;
    open_param.stream_in_param.afe.bias_voltage[0] = HAL_AUDIO_BIAS_VOLTAGE_2_40V;
    open_param.stream_in_param.afe.bias_voltage[1] = HAL_AUDIO_BIAS_VOLTAGE_2_40V;
    open_param.stream_in_param.afe.bias_voltage[2] = HAL_AUDIO_BIAS_VOLTAGE_2_40V;
    open_param.stream_in_param.afe.bias_voltage[3] = HAL_AUDIO_BIAS_VOLTAGE_2_40V;
    open_param.stream_in_param.afe.bias_voltage[4] = HAL_AUDIO_BIAS_VOLTAGE_2_40V;
    open_param.stream_in_param.afe.bias_select = audio_nvdm_HW_config.voice_scenario.Voice_MIC_Bias_Enable;
    open_param.stream_in_param.afe.iir_filter[0] = HAL_AUDIO_UL_IIR_DISABLE;
    open_param.stream_in_param.afe.adc_mode = adc_mode_HQA;
    open_param.stream_in_param.afe.dmic_selection[0] = adc_mode_HQA;
#endif

    open_param.stream_in_param.afe.audio_device = in_audio_device;
    if(open_param.stream_in_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
        open_param.stream_in_param.afe.misc_parms      = I2S_CLK_SOURCE_DCXO;
        if (linein_sample_rate > HAL_AUDIO_SAMPLING_RATE_48KHZ) {
            open_param.stream_in_param.afe.misc_parms  = I2S_CLK_SOURCE_APLL;
        }
    } else {
        if (in_audio_device & HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL) {
            open_param.stream_in_param.afe.misc_parms      = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_2p4v;
        } else {
            open_param.stream_in_param.afe.misc_parms      = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_1p85v;
        }
    }

    open_param.stream_out_param.afe.audio_device = out_audio_device;
    //open_param.stream_out_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
    open_param.stream_out_param.afe.audio_interface = device_in_interface_HQA;
    open_param.stream_out_param.afe.stream_channel = HAL_AUDIO_DIRECT;
    open_param.stream_out_param.afe.format = AFE_PCM_FORMAT_S32_LE;
    open_param.stream_out_param.afe.stream_out_sampling_rate = hal_audio_sampling_rate_enum_to_value(linein_sample_rate);
#if defined (FIXED_SAMPLING_RATE_TO_48KHZ)
    open_param.stream_out_param.afe.sampling_rate   = HAL_AUDIO_FIXED_AFE_48K_SAMPLE_RATE;
#elif defined (AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ)
    open_param.stream_out_param.afe.sampling_rate   = HAL_AUDIO_FIXED_AFE_96K_SAMPLE_RATE;
#else
    open_param.stream_out_param.afe.sampling_rate   = hal_audio_sampling_rate_enum_to_value(linein_sample_rate);
#endif
    open_param.stream_out_param.afe.hw_gain = true;
    if(open_param.stream_out_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
        open_param.stream_out_param.afe.misc_parms      = I2S_CLK_SOURCE_DCXO;
        if (linein_sample_rate > HAL_AUDIO_SAMPLING_RATE_48KHZ) {
            open_param.stream_out_param.afe.misc_parms  = I2S_CLK_SOURCE_APLL;
        }
    } else {
        open_param.stream_out_param.afe.misc_parms      = DOWNLINK_PERFORMANCE_NORMAL;
    }

    open_param.stream_out_param.afe.performance = dac_performance_HQA;
#if defined(HAL_AUDIO_SUPPORT_APLL)
    //Open i2s master in low jitter mode
    hal_audio_i2s_master_in_low_jitter_mode(LOW_JITTER_MODE_PURE_LINEIN_PLAYBACK_IN, in_audio_device, hal_audio_sampling_rate_enum_to_value(linein_sample_rate), open_param.stream_in_param.afe.is_low_jitter, true);
    //Open i2s master out low jitter mode
    hal_audio_i2s_master_out_low_jitter_mode(LOW_JITTER_MODE_PURE_LINEIN_PLAYBACK_OUT, out_audio_device, open_param.stream_out_param.afe.sampling_rate, open_param.stream_out_param.afe.is_low_jitter, true);
#endif

    // Open uplink high res
    hal_audio_dsp_ul_clkmux_control(in_audio_device, hal_audio_sampling_rate_enum_to_value(linein_sample_rate), true);
    // Open downlink high res
    hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_LINEIN, out_audio_device, open_param.stream_out_param.afe.sampling_rate, true);
    if((in_audio_device&HAL_AUDIO_DEVICE_I2S_MASTER)||(out_audio_device&HAL_AUDIO_DEVICE_I2S_MASTER)){
        if(open_param.stream_in_param.afe.audio_interface==HAL_AUDIO_INTERFACE_1){
            hal_gpio_init(8);
            hal_pinmux_set_function(8,4);
            hal_gpio_init(9);
            hal_pinmux_set_function(9,4);
            hal_gpio_init(10);
            hal_pinmux_set_function(10,4);
            hal_gpio_init(11);
            hal_pinmux_set_function(11,4);
            hal_gpio_init(6);
            hal_pinmux_set_function(6,4);//O:I2S_MST0_MCLK
            //hal_audio_mclk_enable(true, AFE_MCLK_PIN_FROM_I2S0, AFE_APLL2, 0);//enable mclk
            LINEIN_PLAYBACK_LOG_I("I2S0 GPIO Set done\r\n",0);
        }else if(open_param.stream_in_param.afe.audio_interface==HAL_AUDIO_INTERFACE_2){
            //I2S1 TX
            hal_gpio_init(13);
            hal_pinmux_set_function(13,4);
            //I2S1 CK//be set by user, because AT CMD(UART2 RX) is occupied.
            //hal_gpio_init(12);
            //hal_pinmux_set_function(12,4);
            //I2S1 WS
            hal_gpio_init(15);
            hal_pinmux_set_function(15,4);
            //I2S1 RX
            hal_gpio_init(21);
            hal_pinmux_set_function(21,2);
            hal_gpio_init(18);
            hal_pinmux_set_function(18,6);//O:I2S_MST1_MCLK
            //hal_audio_mclk_enable(true, AFE_MCLK_PIN_FROM_I2S1, AFE_APLL2, 0);//enable mclk
            LINEIN_PLAYBACK_LOG_I("I2S2 GPIO Set done\r\n",0);
        }else if(open_param.stream_in_param.afe.audio_interface==HAL_AUDIO_INTERFACE_3){
            //I2S2 TX
            hal_gpio_init(19);
            hal_pinmux_set_function(19,4);
            //I2S2 CK
            hal_gpio_init(17);
            hal_pinmux_set_function(17,4);
            //I2S2 WS
            hal_gpio_init(16);
            hal_pinmux_set_function(16,4);
            //I2S2 RX
            hal_gpio_init(18);
            hal_pinmux_set_function(18,4);
            hal_gpio_init(20);
            hal_pinmux_set_function(20,1);//O:I2S_MST2_MCLK
            //hal_audio_mclk_enable(true, AFE_MCLK_PIN_FROM_I2S2, AFE_APLL2, 0);//enable mclk
            LINEIN_PLAYBACK_LOG_I("I2S3 GPIO Set done\r\n",0);
        }

    }
    if(in_audio_device&HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL){
        if((uint32_t)adc_mode_HQA==(uint32_t)HAL_AUDIO_DMIC_GPIO_DMIC0){
            //2822
            //GPIO2 AuxFunc.3 DMIC0_CLK
            //GPIO3 AuxFunc.3 DMIC0_DAT

            hal_gpio_init(2);
            hal_pinmux_set_function(2,3);
            hal_gpio_init(3);
            hal_pinmux_set_function(3,3);

            LINEIN_PLAYBACK_LOG_I("DMIC0 GPIO Set done dmic sel %d\r\n",1,adc_mode_HQA);
        }else if((uint32_t)adc_mode_HQA==(uint32_t)HAL_AUDIO_DMIC_GPIO_DMIC1){
            //2822
            //GPIO4 AuxFunc.3 DMIC0_CLK
            //GPIO5 AuxFunc.3 DMIC0_DAT

            hal_gpio_init(4);
            hal_pinmux_set_function(4,3);
            hal_gpio_init(5);
            hal_pinmux_set_function(5,3);
            LINEIN_PLAYBACK_LOG_I("DMIC1 GPIO Set done dmic sel %d\r\n",1,adc_mode_HQA);
        }
    }


#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
    ami_set_afe_param(STREAM_OUT, linein_sample_rate, true);
#endif
    p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), AUDIO_MESSAGE_TYPE_LINEIN);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_TRULY_LINEIN_PLAYBACK_OPEN, 0, (uint32_t)p_param_share, true);

    linein_status = LINEIN_STATE_PLAY;
    return LINEIN_EXECUTION_SUCCESS;
}
#endif

linein_result_t pure_linein_playback_open(hal_audio_sampling_rate_t linein_sample_rate, hal_audio_device_t in_audio_device, hal_audio_device_t out_audio_device)
{
#if defined(MTK_AVM_DIRECT)
    if(!hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_LINEIN)){
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_LINEIN, true);
    }
#endif
    LINEIN_PLAYBACK_LOG_I("enter pure_linein_playback_open, sample_rate=%d, in_audio_device=0x%x, out_audio_device=0x%x\n", 3, hal_audio_sampling_rate_enum_to_value(linein_sample_rate), in_audio_device, out_audio_device);
    if (linein_status != LINEIN_STATE_IDLE) {
        LINEIN_PLAYBACK_LOG_I("cannot open because state(%d)\n", 1, linein_status);
        return LINEIN_EXECUTION_FAIL;
    }

    if (linein_sample_rate != HAL_AUDIO_SAMPLING_RATE_48KHZ && linein_sample_rate != HAL_AUDIO_SAMPLING_RATE_96KHZ && linein_sample_rate != HAL_AUDIO_SAMPLING_RATE_192KHZ) {
        LINEIN_PLAYBACK_LOG_I("not support sample rate=%d\n", 1, hal_audio_sampling_rate_enum_to_value(linein_sample_rate));
        return LINEIN_EXECUTION_FAIL;
    }
    if (in_audio_device != HAL_AUDIO_DEVICE_I2S_MASTER && in_audio_device != HAL_AUDIO_DEVICE_I2S_SLAVE && ((in_audio_device & HAL_AUDIO_DEVICE_MAIN_MIC_DUAL) == false)
        && ((in_audio_device & HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL) == false) && ((in_audio_device & HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL) == false)) {
        LINEIN_PLAYBACK_LOG_I("not support in device=%d\n", 1, in_audio_device);
        return LINEIN_EXECUTION_FAIL;
    }
    if (out_audio_device != HAL_AUDIO_DEVICE_I2S_MASTER && out_audio_device != HAL_AUDIO_DEVICE_I2S_SLAVE && ((out_audio_device & HAL_AUDIO_DEVICE_DAC_DUAL) == false)) {
        LINEIN_PLAYBACK_LOG_I("not support out device=%d\n", 1, out_audio_device);
        return LINEIN_EXECUTION_FAIL;
    }

    mcu2dsp_open_param_t open_param;
    memset(&open_param, 0, sizeof(mcu2dsp_open_param_t));
    void *p_param_share;

    open_param.param.stream_in = STREAM_IN_AFE;
    open_param.param.stream_out = STREAM_OUT_AFE;

    #ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        open_param.stream_in_param.afe.audio_interface = hal_audio_convert_linein_interface(audio_nvdm_HW_config.audio_scenario.Audio_Linein_Input_Path,true);
    #else
    open_param.stream_in_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
    #endif

#ifdef ENABLE_2A2D_TEST
    #ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        open_param.stream_in_param.afe.audio_interface1 = hal_audio_convert_linein_interface(audio_nvdm_HW_config.audio_scenario.Audio_Linein_Input_Path,true);
    #endif
#endif

#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    open_param.stream_in_param.afe.performance = audio_nvdm_HW_config.audio_scenario.Audio_Analog_LineIn_Performance_Sel;
    open_param.stream_in_param.afe.iir_filter[0] = audio_nvdm_HW_config.adc_dac_config.ADDA_Audio_IIR_Filter;
    open_param.stream_in_param.afe.adc_mode = audio_nvdm_HW_config.adc_dac_config.ADDA_Analog_MIC0_Mode;
#endif

    open_param.stream_in_param.afe.audio_device = in_audio_device;
    #ifdef ENABLE_2A2D_TEST
        #ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
            open_param.stream_in_param.afe.audio_device1 = in_audio_device;
        #endif
    #endif

    if(open_param.stream_in_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
        open_param.stream_in_param.afe.misc_parms      = I2S_CLK_SOURCE_DCXO;
        if (linein_sample_rate > HAL_AUDIO_SAMPLING_RATE_48KHZ) {
            open_param.stream_in_param.afe.misc_parms  = I2S_CLK_SOURCE_APLL;
        }
    } else {
        if (in_audio_device & HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL) {
            open_param.stream_in_param.afe.misc_parms      = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_2p4v;
        } else {
            open_param.stream_in_param.afe.misc_parms      = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_1p85v;
        }
    }

    open_param.stream_out_param.afe.audio_device = out_audio_device;
    open_param.stream_out_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
    open_param.stream_out_param.afe.stream_channel = HAL_AUDIO_DIRECT;
    open_param.stream_out_param.afe.format = AFE_PCM_FORMAT_S32_LE;
    open_param.stream_out_param.afe.stream_out_sampling_rate = hal_audio_sampling_rate_enum_to_value(linein_sample_rate);
#if defined (FIXED_SAMPLING_RATE_TO_48KHZ)
    open_param.stream_out_param.afe.sampling_rate   = HAL_AUDIO_FIXED_AFE_48K_SAMPLE_RATE;
#elif defined (AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ)
    open_param.stream_out_param.afe.sampling_rate   = HAL_AUDIO_FIXED_AFE_96K_SAMPLE_RATE;
#else
    open_param.stream_out_param.afe.sampling_rate   = hal_audio_sampling_rate_enum_to_value(linein_sample_rate);
#endif
    open_param.stream_out_param.afe.hw_gain = true;

#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    open_param.stream_out_param.afe.performance = audio_nvdm_HW_config.adc_dac_config.ADDA_DAC_Class_AB_G_Performance_Sel;
#endif
    if(open_param.stream_out_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
        open_param.stream_out_param.afe.misc_parms      = I2S_CLK_SOURCE_DCXO;
        if (linein_sample_rate > HAL_AUDIO_SAMPLING_RATE_48KHZ) {
            open_param.stream_out_param.afe.misc_parms  = I2S_CLK_SOURCE_APLL;
        }
    } else {
        open_param.stream_out_param.afe.misc_parms      = DOWNLINK_PERFORMANCE_NORMAL;
    }
#if defined(HAL_AUDIO_SUPPORT_APLL)
    //Open i2s master in low jitter mode
    hal_audio_i2s_master_in_low_jitter_mode(LOW_JITTER_MODE_PURE_LINEIN_PLAYBACK_IN, in_audio_device, hal_audio_sampling_rate_enum_to_value(linein_sample_rate), open_param.stream_in_param.afe.is_low_jitter, true);
    //Open i2s master out low jitter mode
    hal_audio_i2s_master_out_low_jitter_mode(LOW_JITTER_MODE_PURE_LINEIN_PLAYBACK_OUT, out_audio_device, open_param.stream_out_param.afe.sampling_rate, open_param.stream_out_param.afe.is_low_jitter, true);
#endif

    // Open uplink high res
    hal_audio_dsp_ul_clkmux_control(in_audio_device, hal_audio_sampling_rate_enum_to_value(linein_sample_rate), true);
    // Open downlink high res
    hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_LINEIN, out_audio_device, open_param.stream_out_param.afe.sampling_rate, true);

#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
    ami_set_afe_param(STREAM_OUT, linein_sample_rate, true);
#endif
    p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), AUDIO_MESSAGE_TYPE_LINEIN);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_TRULY_LINEIN_PLAYBACK_OPEN, 0, (uint32_t)p_param_share, true);

    linein_status = LINEIN_STATE_PLAY;
    return LINEIN_EXECUTION_SUCCESS;
}

linein_result_t pure_linein_playback_close()
{
    LINEIN_PLAYBACK_LOG_I("enter pure_linein_playback_close\n", 0);
    if (linein_status != LINEIN_STATE_PLAY) {
        LINEIN_PLAYBACK_LOG_I("cannot close because state(%d)\n", 1, linein_status);
        return LINEIN_EXECUTION_FAIL;
    }
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_TRULY_LINEIN_PLAYBACK_CLOSE, 0, 0, true);

#if defined(HAL_AUDIO_SUPPORT_APLL)
    //Close i2s master out low jitter mode
    hal_audio_i2s_master_out_low_jitter_mode(LOW_JITTER_MODE_PURE_LINEIN_PLAYBACK_OUT, 0, 0, 0, false);
    //Close i2s master in low jitter mode
    hal_audio_i2s_master_in_low_jitter_mode(LOW_JITTER_MODE_PURE_LINEIN_PLAYBACK_IN, 0, 0, 0, false);
#endif

    // Close downlink high res
    hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_LINEIN, 0, 0, false);
    // Close uplink high res
    hal_audio_dsp_ul_clkmux_control(0, 0, false);

    linein_status = LINEIN_STATE_IDLE;
#if defined(MTK_AVM_DIRECT)
    if(hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_LINEIN)){
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_LINEIN, false);
    }
#endif
    return LINEIN_EXECUTION_SUCCESS;
}

linein_result_t audio_linein_playback_open(hal_audio_sampling_rate_t linein_sample_rate, hal_audio_device_t in_audio_device, hal_audio_device_t out_audio_device)
{
#if defined(MTK_PURE_LINEIN_PLAYBACK_ENABLE)
    return pure_linein_playback_open(linein_sample_rate, in_audio_device, out_audio_device);
#else
    #ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        if (0x01 == audio_nvdm_HW_config.audio_scenario.Audio_Pure_Linein_enable) {
            return pure_linein_playback_open(linein_sample_rate, in_audio_device, out_audio_device);
        }else {
    return linein_playback_open(linein_sample_rate, in_audio_device, out_audio_device);
        }
    #else
        return linein_playback_open(linein_sample_rate, in_audio_device, out_audio_device);
    #endif
#endif
}

linein_result_t audio_linein_playback_start()
{
#if defined(MTK_PURE_LINEIN_PLAYBACK_ENABLE)
    return LINEIN_EXECUTION_SUCCESS;
#else
    #ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        if (0x01 == audio_nvdm_HW_config.audio_scenario.Audio_Pure_Linein_enable) {
            return LINEIN_EXECUTION_SUCCESS;
        }else {
    return linein_playback_start();
        }
    #else
        return linein_playback_start();
    #endif
#endif
}

linein_result_t audio_linein_playback_stop()
{
#if defined(MTK_PURE_LINEIN_PLAYBACK_ENABLE)
    return LINEIN_EXECUTION_SUCCESS;
#else
    #ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        if (0x01 == audio_nvdm_HW_config.audio_scenario.Audio_Pure_Linein_enable) {
            return LINEIN_EXECUTION_SUCCESS;
        }else {
    return linein_playback_stop();
        }
    #else
        return linein_playback_stop();
    #endif
#endif
}

linein_result_t audio_linein_playback_close()
{
#if defined(MTK_PURE_LINEIN_PLAYBACK_ENABLE)
    return pure_linein_playback_close();
#else
    #ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        if (0x01 == audio_nvdm_HW_config.audio_scenario.Audio_Pure_Linein_enable) {
            return pure_linein_playback_close();
        }else {
            return linein_playback_close();
        }
    #else
    return linein_playback_close();
    #endif
#endif
}

linein_result_t audio_linein_playback_set_volume(linein_playback_gain_t gain)
{
    return linein_playback_set_volume(gain);
}

linein_result_t audio_pure_linein_playback_open(hal_audio_sampling_rate_t linein_sample_rate, hal_audio_device_t in_audio_device, hal_audio_device_t out_audio_device)
{
    return pure_linein_playback_open(linein_sample_rate, in_audio_device, out_audio_device);
}

linein_result_t audio_pure_linein_playback_close()
{
    return pure_linein_playback_close();
}

#ifdef LINE_IN_PURE_FOR_AMIC_CLASS_G_HQA
linein_result_t audio_pure_linein_playback_open_HQA(hal_audio_sampling_rate_t linein_sample_rate, hal_audio_device_t in_audio_device,hal_audio_interface_t device_in_interface_HQA,hal_audio_analog_mdoe_t adc_mode_HQA,hal_audio_performance_mode_t mic_performance_HQA, hal_audio_device_t out_audio_device,hal_audio_performance_mode_t dac_performance_HQA)
{
    return pure_linein_playback_open_HQA( linein_sample_rate,  in_audio_device, device_in_interface_HQA, adc_mode_HQA, mic_performance_HQA,  out_audio_device, dac_performance_HQA);
}
#endif

