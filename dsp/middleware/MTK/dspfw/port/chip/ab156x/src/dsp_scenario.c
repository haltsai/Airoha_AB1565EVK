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

/******************************************************************************
* Include
******************************************************************************/
#include "string.h"
#include "dsp_scenario.h"
#include "bt_interface.h"
#include "stream.h"
#ifdef MTK_BT_HFP_ENABLE
#include "stream_n9sco.h"
#endif
#include "stream_n9_a2dp.h"
#include "hal_audio_afe_define.h"
#include "hal_audio_afe_control.h"
#include "hal_sleep_manager.h"
#include "dsp_audio_msg_define.h"
#include "dsp_drv_afe.h"
#include "davt.h"
#include "dhpt.h"
#include "dllt.h"
#include "dsp_audio_ctrl.h"
#include "dsp_share_memory.h"
#include "dsp_temp.h"
#include "audio_nvdm_common.h"
#include "clk_skew.h"
#include "bt_interface.h"
#ifdef AIR_VOICE_NR_ENABLE
#include "aec_nr_interface.h"
#endif
#include "compander_interface.h"
#include "long_term_clk_skew.h"
#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
#include "peq_interface.h"
#include "compander_interface.h"
#endif
#include "dsp_dump.h"
#include "sfr_bt.h"
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
#include "leakage_compensation.h"
#endif
#ifdef MTK_WWE_ENABLE
#include "wwe_interface.h"
#endif
#ifdef MTK_SENSOR_SOURCE_ENABLE
#include "bsp_multi_axis_sensor.h"
#endif
#ifdef MTK_AUDIO_PLC_ENABLE
#include "audio_plc_interface.h"
#endif
#include "dsp_audio_msg_define.h"

#ifdef MTK_BT_A2DP_ENABLE
#define PLAY_EN_CHECK_TIMER
#endif

#define TRIGGER_A2DP_PROCSESS_TIMER
#if defined(PLAY_EN_CHECK_TIMER) || defined(TRIGGER_A2DP_PROCSESS_TIMER)
#include "timers.h"
#endif

#ifdef MTK_CELT_DEC_ENABLE
#include "celt_dec_interface.h"
#endif

#ifdef AIR_BT_CODEC_BLE_ENABLED
#include "stream_n9ble.h"
#include "common.h"
//#include "lc3_codec_api.h"
#include "lc3_dec_interface.h"
#include "lc3_enc_interface.h"
#endif

#ifdef MTK_3RD_PARTY_NR
#include "tx_eq_interface.h"
#endif
#ifdef MTK_LINEIN_INS_ENABLE
#include "ins_interface.h"
#endif

#ifdef MTK_AUDIO_LOOPBACK_TEST_ENABLE
#include "audio_loopback_test_interface.h"
#endif

#ifdef AIR_SOFTWARE_SRC_ENABLE
#include "sw_src_interface.h"
#endif
#ifdef AIR_SOFTWARE_CLK_SKEW_ENABLE
#include "clk_skew_sw.h"
#endif
#ifdef AIR_SOFTWARE_GAIN_ENABLE
#include "sw_gain_interface.h"
#endif
#ifdef AIR_ANC_WIND_DETECTION_ENABLE
#include "wind_interface.h"
#endif
#if defined(AIR_WIRED_AUDIO_ENABLE)
#include "scenario_wired_audio.h"
#endif
#ifdef MTK_ANC_ENABLE
#include "anc_api.h"
#endif
#ifdef MTK_USER_TRIGGER_FF_ENABLE
#include "user_trigger_adaptive_ff.h"
#endif
#ifdef AIR_ANC_USER_UNAWARE_ENABLE
#include "user_unaware_interface.h"
#endif
#include "hal_audio_path.h"
#include "hal_audio.h"
#ifdef AIR_SILENCE_DETECTION_ENABLE
#include "silence_detection_interface.h"
#endif
#include "dtm.h"
#ifdef AIR_FIXED_RATIO_SRC
#include "src_fixed_ratio_interface.h"
#endif

#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
#include "environment_detection_interface.h"
#endif

#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
#include "scenario_advanced_passthrough.h"
#endif

/******************************************************************************
* Define
******************************************************************************/
#define DSP_REMAP_SHARE_INFO(para,type)  ({  \
         para = (type)hal_memview_cm4_to_dsp0((uint32_t)para); \
         /*((SHARE_BUFFER_INFO_PTR) para)->startaddr = hal_memview_cm4_to_dsp0(((SHARE_BUFFER_INFO_PTR) para)->startaddr);*/})
#define abs32(x) ( (x >= 0) ? x : (-x) )

EXTERN audio_hardware afe_get_audio_hardware_by_au_afe_open_param (au_afe_open_param_t afe_open);
EXTERN audio_instance afe_get_audio_instance_by_au_afe_open_param (au_afe_open_param_t afe_open);
EXTERN audio_channel afe_get_audio_channel_by_au_afe_open_param (au_afe_open_param_t afe_open);
EXTERN afe_t afe;
EXTERN void hal_audio_afe_disable_play_en(void);


/******************************************************************************
* Global variable
******************************************************************************/
CONNECTION_IF playback_if, n9_a2dp_if, record_if, linein_playback_if;
#ifdef AIR_BT_CODEC_BLE_ENABLED
CONNECTION_IF n9_ble_ul_if, n9_ble_dl_if;
#endif
#ifdef MTK_PROMPT_SOUND_ENABLE
CONNECTION_IF playback_vp_if;
#endif
#ifdef AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
CONNECTION_IF playback_vp_dummy_source_if;
#endif
#ifdef MTK_SENSOR_SOURCE_ENABLE
CONNECTION_IF sensor_src_if[AUDIO_TRANSMITTER_GSENSOR_SUB_ID_MAX] =
{/*source,      sink,  transform, pfeature_table;*/
    {NULL,      NULL,       NULL,     AudioFeatureList_GSensorMotionDetect_virtual},  //no need to send data to MCU
    {NULL,      NULL,       NULL,     AudioFeatureList_GSensorMotionDetect}, //send sensor data to MCU
    {NULL,      NULL,       NULL,     AudioFeatureList_GSensorMotionDetect_virtual},
};
#endif

#if defined(MTK_ANC_SURROUND_MONITOR_ENABLE)
CONNECTION_IF sensor_anc_monitor_if = {NULL,      NULL,       NULL,     stream_featuremulti_anc_monitor};
#endif

void afe_print_reg(void){
     DSP_MW_LOG_I("AUDIO_TOP_CON0=%x\r\n", 1, AFE_READ(AUDIO_TOP_CON0));
     DSP_MW_LOG_I("AUDIO_TOP_CON1=%x\r\n", 1, AFE_READ(AUDIO_TOP_CON1));
     DSP_MW_LOG_I("AFE_DAC_CON0=%x\r\n", 1, AFE_READ(AFE_DAC_CON0));
     DSP_MW_LOG_I("AFE_DAC_CON1=%x\r\n", 1, AFE_READ(AFE_DAC_CON1));
     DSP_MW_LOG_I("AFE_DAC_CON2=%x\r\n", 1, AFE_READ(AFE_DAC_CON2));
     DSP_MW_LOG_I("AFE_ADDA_TOP_CON0=%x\r\n", 1, AFE_READ(AFE_ADDA_TOP_CON0));
     DSP_MW_LOG_I("AFE_ADDA_DL_SRC2_CON0=%x\r\n", 1, AFE_READ(AFE_ADDA_DL_SRC2_CON0));
     DSP_MW_LOG_I("AFE_ADDA_DL_SRC2_CON1=%x\r\n", 1, AFE_READ(AFE_ADDA_DL_SRC2_CON1));
     DSP_MW_LOG_I("AFE_ADDA_UL_DL_CON0=%x\r\n", 1, AFE_READ(AFE_ADDA_UL_DL_CON0));
     DSP_MW_LOG_I("AFE_ADDA_PREDIS_CON0=%x\r\n", 1, AFE_READ(AFE_ADDA_PREDIS_CON0));
     DSP_MW_LOG_I("AFE_ADDA_PREDIS_CON1=%x\r\n", 1, AFE_READ(AFE_ADDA_PREDIS_CON1));
     DSP_MW_LOG_I("AFE_CLASSG_LPSRCH_CFG0=%x\r\n", 1, AFE_READ(AFE_CLASSG_LPSRCH_CFG0));
     DSP_MW_LOG_I("ZCD_CON2=%x\r\n", 1, AFE_READ(ZCD_CON2));
     DSP_MW_LOG_I("ZCD_CON0=%x\r\n", 1, AFE_READ(ZCD_CON0));
     //DSP_MW_LOG_I("ZCD_CON1=%x\r\n", 1, AFE_READ(ZCD_CON1));
     DSP_MW_LOG_I("AUDDEC_ANA_CON0=%x\r\n", 1, ANA_READ(AUDDEC_ANA_CON0));
     DSP_MW_LOG_I("AUDDEC_ANA_CON1=%x\r\n", 1, ANA_READ(AUDDEC_ANA_CON1));
     DSP_MW_LOG_I("AUDDEC_ANA_CON2=%x\r\n", 1, ANA_READ(AUDDEC_ANA_CON2));
     DSP_MW_LOG_I("AUDDEC_ANA_CON3=%x\r\n", 1, ANA_READ(AUDDEC_ANA_CON3));
     DSP_MW_LOG_I("AUDDEC_ANA_CON4=%x\r\n", 1, ANA_READ(AUDDEC_ANA_CON4));
     DSP_MW_LOG_I("AUDDEC_ANA_CON5=%x\r\n", 1, ANA_READ(AUDDEC_ANA_CON5));
     DSP_MW_LOG_I("AUDDEC_ANA_CON6=%x\r\n", 1, ANA_READ(AUDDEC_ANA_CON6));
     DSP_MW_LOG_I("AUDDEC_ANA_CON7=%x\r\n", 1, ANA_READ(AUDDEC_ANA_CON7));
     DSP_MW_LOG_I("AUDDEC_ANA_CON8=%x\r\n", 1, ANA_READ(AUDDEC_ANA_CON8));
     DSP_MW_LOG_I("AUDDEC_ANA_CON9=%x\r\n", 1, ANA_READ(AUDDEC_ANA_CON9));
     DSP_MW_LOG_I("AUDDEC_ANA_CON10=%x\r\n", 1, ANA_READ(AUDDEC_ANA_CON10));
     DSP_MW_LOG_I("AUDDEC_ANA_CON11=%x\r\n", 1, ANA_READ(AUDDEC_ANA_CON11));
     DSP_MW_LOG_I("AUDDEC_ANA_CON12=%x\r\n", 1, ANA_READ(AUDDEC_ANA_CON12));
     DSP_MW_LOG_I("AUDDEC_ANA_CON13=%x\r\n", 1, ANA_READ(AUDDEC_ANA_CON13));
     DSP_MW_LOG_I("AUDDEC_ANA_CON14=%x\r\n", 1, ANA_READ(AUDDEC_ANA_CON14));
}
bool BleDlStopFlag;
audio_dsp_ull_start_ctrl_param_t audio_headset_ull_ctrl;
#define ULL_BASIC_LATENCY 20000

/******************************************************************************
* Function
******************************************************************************/
static void dsp_audio_msg_ack(uint32_t msg_id, bool from_isr);
U32 dsp_calcuate_number_of_bit (U32 value)
{
    U32 number_of_bit;
    value = value - ((value >> 1) & 0x55555555);
    value = (value & 0x33333333) + ((value >> 2) & 0x33333333);
    number_of_bit = (((value + (value >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
    return number_of_bit;
}

#ifdef AIR_AUDIO_HARDWARE_ENABLE
SOURCE dsp_open_stream_in_afe(mcu2dsp_open_param_p open_param)
{
    SOURCE source;
    bool echo_path = false;
    #ifdef AIR_DONGLE_AFE_USAGE_CHECK_ENABLE
    DSP_MW_LOG_I("AFE USAGE CHECK, AFE in exist", 0);
    configASSERT(0);
    #endif
    //DSP_MW_LOG_I("Stream in afe\r\n", 0);
    DSP_MW_LOG_I("afe in device:%d, channel:%d, memory:%d, interface:%d \r\n", 4, open_param->stream_in_param.afe.audio_device,
                                                                                  open_param->stream_in_param.afe.stream_channel,
                                                                                  open_param->stream_in_param.afe.memory,
                                                                                  open_param->stream_in_param.afe.audio_interface);

    DSP_MW_LOG_I("afe in interface:%d, hw_gain:%d\r\n", 2, open_param->stream_in_param.afe.audio_interface,
                                                  open_param->stream_in_param.afe.hw_gain);
#ifdef ENABLE_2A2D_TEST
	DSP_MW_LOG_I("[2A2D] afe in audio_device: %d, %d, %d, %d\r\n",4,open_param->stream_in_param.afe.audio_device, open_param->stream_in_param.afe.audio_device1, open_param->stream_in_param.afe.audio_device2, open_param->stream_in_param.afe.audio_device3);
    DSP_MW_LOG_I("[2A2D] afe in audio_interface: %d, %d, %d, %d\r\n", 4, open_param->stream_in_param.afe.audio_interface, open_param->stream_in_param.afe.audio_interface1, open_param->stream_in_param.afe.audio_interface2, open_param->stream_in_param.afe.audio_interface3);
#endif

    AudioAfeConfiguration(AUDIO_SOURCE_DATA_FORMAT, open_param->stream_in_param.afe.format);

    if (open_param->stream_in_param.afe.sampling_rate != 0) {
        AudioAfeConfiguration(AUDIO_SOURCE_IRQ_RATE, open_param->stream_in_param.afe.sampling_rate);
    }
    #ifdef AIR_HWSRC_RX_TRACKING_ENABLE
    if (open_param->stream_in_param.afe.stream_out_sampling_rate != 0) {
        AudioAfeConfiguration(AUDIO_SRC_RATE, open_param->stream_in_param.afe.stream_out_sampling_rate);
        DSP_MW_LOG_I("[HWSRC] hwsrc output rate = %d",1,open_param->stream_in_param.afe.stream_out_sampling_rate);
    }
    #endif
    AudioAfeConfiguration(AUDIO_SOURCE_IRQ_PERIOD, open_param->stream_in_param.afe.irq_period);
    #if defined(AIR_ULL_VOICE_LOW_LATENCY_ENABLE)
    gAudioCtrl.Afe.AfeULSetting.period = open_param->stream_in_param.afe.irq_period;
    #endif

    /*echo path*/
    echo_path = (open_param->stream_in_param.afe.memory&HAL_AUDIO_MEM3)/* && (dsp_calcuate_number_of_bit(open_param->stream_in_param.afe.audio_interface) == 1)*/;
    if (echo_path) {
        open_param->stream_in_param.afe.memory &= ~HAL_AUDIO_MEM3;//echo path
        DSP_MW_LOG_I("afe in with echo, memory:%d\r\n", 1, open_param->stream_in_param.afe.memory);
    }else{
        DSP_MW_LOG_I("afe in not with echo, memory:%d\r\n", 1, open_param->stream_in_param.afe.memory);
    }


#if 0

    /* 4 A-MIC Verification */
    open_param->stream_in_param.afe.audio_device        = HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_L;
    open_param->stream_in_param.afe.audio_device1       = HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_R;
    open_param->stream_in_param.afe.audio_device2       = HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_L;
    open_param->stream_in_param.afe.audio_device3       = HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_R;

    open_param->stream_in_param.afe.audio_interface     = HAL_AUDIO_INTERFACE_1;
    open_param->stream_in_param.afe.audio_interface1    = HAL_AUDIO_INTERFACE_1;
    open_param->stream_in_param.afe.audio_interface2    = HAL_AUDIO_INTERFACE_2;
    open_param->stream_in_param.afe.audio_interface3    = HAL_AUDIO_INTERFACE_2;

#endif

    AudioAfeConfiguration(AUDIO_SOURCE_DEVICE, open_param->stream_in_param.afe.audio_device);
    DSP_MW_LOG_I("audio_device 0x%x",1,open_param->stream_in_param.afe.audio_device);
#ifdef ENABLE_2A2D_TEST
    AudioAfeConfiguration(AUDIO_SOURCE_DEVICE1, open_param->stream_in_param.afe.audio_device1);
    DSP_MW_LOG_I("audio_device1 0x%x",1,open_param->stream_in_param.afe.audio_device1);
    AudioAfeConfiguration(AUDIO_SOURCE_DEVICE2, open_param->stream_in_param.afe.audio_device2);
    DSP_MW_LOG_I("audio_device2 0x%x",1,open_param->stream_in_param.afe.audio_device2);
    AudioAfeConfiguration(AUDIO_SOURCE_DEVICE3, open_param->stream_in_param.afe.audio_device3);
    DSP_MW_LOG_I("audio_device3 0x%x",1,open_param->stream_in_param.afe.audio_device3);
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    AudioAfeConfiguration(AUDIO_SOURCE_DEVICE4, open_param->stream_in_param.afe.audio_device4);
    DSP_MW_LOG_I("audio_device4 0x%x",1,open_param->stream_in_param.afe.audio_device4);
    AudioAfeConfiguration(AUDIO_SOURCE_DEVICE5, open_param->stream_in_param.afe.audio_device5);
    DSP_MW_LOG_I("audio_device5 0x%x",1,open_param->stream_in_param.afe.audio_device5);
    AudioAfeConfiguration(AUDIO_SOURCE_DEVICE6, open_param->stream_in_param.afe.audio_device6);
    DSP_MW_LOG_I("audio_device6 0x%x",1,open_param->stream_in_param.afe.audio_device6);
    AudioAfeConfiguration(AUDIO_SOURCE_DEVICE7, open_param->stream_in_param.afe.audio_device7);
    DSP_MW_LOG_I("audio_device7 0x%x",1,open_param->stream_in_param.afe.audio_device7);
#endif
#endif
    AudioAfeConfiguration(AUDIO_SOURCE_CHANNEL, open_param->stream_in_param.afe.stream_channel);
    AudioAfeConfiguration(AUDIO_SOURCE_MEMORY, open_param->stream_in_param.afe.memory);
    AudioAfeConfiguration(AUDIO_SOURCE_INTERFACE, open_param->stream_in_param.afe.audio_interface);
    DSP_MW_LOG_I("audio_interface 0x%x",1,open_param->stream_in_param.afe.audio_interface);
#ifdef ENABLE_2A2D_TEST
    AudioAfeConfiguration(AUDIO_SOURCE_INTERFACE1, open_param->stream_in_param.afe.audio_interface1);
    DSP_MW_LOG_I("audio_interface1 0x%x",1,open_param->stream_in_param.afe.audio_interface1);
    AudioAfeConfiguration(AUDIO_SOURCE_INTERFACE2, open_param->stream_in_param.afe.audio_interface2);
    DSP_MW_LOG_I("audio_interface2 0x%x",1,open_param->stream_in_param.afe.audio_interface2);
    AudioAfeConfiguration(AUDIO_SOURCE_INTERFACE3, open_param->stream_in_param.afe.audio_interface3);
    DSP_MW_LOG_I("audio_interface3 0x%x",1,open_param->stream_in_param.afe.audio_interface3);
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    AudioAfeConfiguration(AUDIO_SOURCE_INTERFACE4, open_param->stream_in_param.afe.audio_interface4);
    DSP_MW_LOG_I("audio_interface4 0x%x",1,open_param->stream_in_param.afe.audio_interface4);
    AudioAfeConfiguration(AUDIO_SOURCE_INTERFACE5, open_param->stream_in_param.afe.audio_interface5);
    DSP_MW_LOG_I("audio_interface5 0x%x",1,open_param->stream_in_param.afe.audio_interface5);
    AudioAfeConfiguration(AUDIO_SOURCE_INTERFACE6, open_param->stream_in_param.afe.audio_interface6);
    DSP_MW_LOG_I("audio_interface6 0x%x",1,open_param->stream_in_param.afe.audio_interface6);
    AudioAfeConfiguration(AUDIO_SOURCE_INTERFACE7, open_param->stream_in_param.afe.audio_interface7);
    DSP_MW_LOG_I("audio_interface7 0x%x",1,open_param->stream_in_param.afe.audio_interface7);
#endif
#endif
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    AudioAfeConfiguration(AUDIO_SOURCE_ADC_MODE, open_param->stream_in_param.afe.ul_adc_mode[0]);
#ifdef ENABLE_2A2D_TEST
    AudioAfeConfiguration(AUDIO_SOURCE_ADC_MODE1, open_param->stream_in_param.afe.ul_adc_mode[1]);
    AudioAfeConfiguration(AUDIO_SOURCE_ADC_MODE2, open_param->stream_in_param.afe.ul_adc_mode[2]);
    AudioAfeConfiguration(AUDIO_SOURCE_ADC_MODE3, open_param->stream_in_param.afe.ul_adc_mode[3]);
    AudioAfeConfiguration(AUDIO_SOURCE_ADC_MODE4, open_param->stream_in_param.afe.ul_adc_mode[4]);
    AudioAfeConfiguration(AUDIO_SOURCE_ADC_MODE5, open_param->stream_in_param.afe.ul_adc_mode[5]);
    AudioAfeConfiguration(AUDIO_SOURCE_ADC_MODE6, open_param->stream_in_param.afe.ul_adc_mode[6]);
    AudioAfeConfiguration(AUDIO_SOURCE_ADC_MODE7, open_param->stream_in_param.afe.ul_adc_mode[7]);
#endif

    AudioAfeConfiguration(AUDIO_SOURCE_BIAS_VOLTAGE, open_param->stream_in_param.afe.bias_voltage[0]);
#ifdef ENABLE_2A2D_TEST
    AudioAfeConfiguration(AUDIO_SOURCE_BIAS_VOLTAGE1, open_param->stream_in_param.afe.bias_voltage[1]);
    AudioAfeConfiguration(AUDIO_SOURCE_BIAS_VOLTAGE2, open_param->stream_in_param.afe.bias_voltage[2]);
    AudioAfeConfiguration(AUDIO_SOURCE_BIAS_VOLTAGE3, open_param->stream_in_param.afe.bias_voltage[3]);
    AudioAfeConfiguration(AUDIO_SOURCE_BIAS_VOLTAGE4, open_param->stream_in_param.afe.bias_voltage[4]);
#endif
    AudioAfeConfiguration(AUDIO_SOURCE_BIAS_SELECT, open_param->stream_in_param.afe.bias_select);
    AudioAfeConfiguration(AUDIO_SOURCE_WITH_EXTERNAL_BIAS, open_param->stream_in_param.afe.with_external_bias);
    AudioAfeConfiguration(AUDIO_SOURCE_WITH_BIAS_LOWPOWER, open_param->stream_in_param.afe.with_bias_lowpower);
    AudioAfeConfiguration(AUDIO_SOURCE_BIAS1_BIAS2_WITH_LDO0, open_param->stream_in_param.afe.bias1_2_with_LDO0);
    AudioAfeConfiguration(AUDIO_SOURCE_DMIC_SELECT, open_param->stream_in_param.afe.dmic_selection[0]);
#ifdef ENABLE_2A2D_TEST
    AudioAfeConfiguration(AUDIO_SOURCE_DMIC_SELECT1, open_param->stream_in_param.afe.dmic_selection[1]);
    AudioAfeConfiguration(AUDIO_SOURCE_DMIC_SELECT2, open_param->stream_in_param.afe.dmic_selection[2]);
    AudioAfeConfiguration(AUDIO_SOURCE_DMIC_SELECT3, open_param->stream_in_param.afe.dmic_selection[3]);
    AudioAfeConfiguration(AUDIO_SOURCE_DMIC_SELECT4, open_param->stream_in_param.afe.dmic_selection[4]);
    AudioAfeConfiguration(AUDIO_SOURCE_DMIC_SELECT5, open_param->stream_in_param.afe.dmic_selection[5]);
    AudioAfeConfiguration(AUDIO_SOURCE_DMIC_SELECT6, open_param->stream_in_param.afe.dmic_selection[6]);
    AudioAfeConfiguration(AUDIO_SOURCE_DMIC_SELECT7, open_param->stream_in_param.afe.dmic_selection[7]);
    DSP_MW_LOG_I("dmic_selection=%d,%d,%d,%d,%d,%d,%d,%d",8,open_param->stream_in_param.afe.dmic_selection[0],
        open_param->stream_in_param.afe.dmic_selection[1],
        open_param->stream_in_param.afe.dmic_selection[2],
        open_param->stream_in_param.afe.dmic_selection[3],
        open_param->stream_in_param.afe.dmic_selection[4],
        open_param->stream_in_param.afe.dmic_selection[5],
        open_param->stream_in_param.afe.dmic_selection[6],
        open_param->stream_in_param.afe.dmic_selection[7]);
#endif
    AudioAfeConfiguration(AUDIO_SOURCE_UL_IIR, open_param->stream_in_param.afe.iir_filter[0]);
    AudioAfeConfiguration(AUDIO_SOURCE_DMIC_CLOCK, open_param->stream_in_param.afe.dmic_clock_rate[0]);

#ifdef ENABLE_2A2D_TEST
    AudioAfeConfiguration(AUDIO_SOURCE_UL_IIR1, open_param->stream_in_param.afe.iir_filter[1]);
    AudioAfeConfiguration(AUDIO_SOURCE_UL_IIR2, open_param->stream_in_param.afe.iir_filter[2]);
    AudioAfeConfiguration(AUDIO_SOURCE_DMIC_CLOCK1, open_param->stream_in_param.afe.dmic_clock_rate[1]);
    AudioAfeConfiguration(AUDIO_SOURCE_DMIC_CLOCK2, open_param->stream_in_param.afe.dmic_clock_rate[2]);
#endif
    AudioAfeConfiguration(AUDIO_SOURCE_UL_PERFORMANCE, open_param->stream_in_param.afe.performance);
    if (open_param->stream_in_param.afe.audio_device & (HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER | HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_L | HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_R)) {
        AudioAfeConfiguration(AUDIO_SOURCE_I2S_FORMAT, open_param->stream_in_param.afe.i2s_format);
        AudioAfeConfiguration(AUDIO_SOURCE_I2S_WORD_LENGTH, open_param->stream_in_param.afe.i2s_word_length);
    } else if (open_param->stream_in_param.afe.audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        AudioAfeConfiguration(AUDIO_SOURCE_I2S_FORMAT, open_param->stream_in_param.afe.i2s_format);
        AudioAfeConfiguration(AUDIO_SOURCE_I2S_SLAVE_TDM, open_param->stream_in_param.afe.i2S_Slave_TDM);
        AudioAfeConfiguration(AUDIO_SOURCE_I2S_WORD_LENGTH, open_param->stream_in_param.afe.i2s_word_length);
    }
    AudioAfeConfiguration(AUDIO_SOURCE_I2S_LOW_JITTER, open_param->stream_in_param.afe.is_low_jitter);
#endif

    AudioAfeConfiguration(AUDIO_SOURCE_HW_GAIN, open_param->stream_in_param.afe.hw_gain);
    #if AUTO_ERROR_SUPPRESSION
    AudioAfeConfiguration(AUDIO_SOURCE_MISC_PARMS_I2S_CLK, open_param->stream_in_param.afe.misc_parms.I2sClkSourceType);
    AudioAfeConfiguration(AUDIO_SOURCE_MISC_PARMS_MICBIAS, open_param->stream_in_param.afe.misc_parms.MicbiasSourceType);
    #endif
    AudioAfeConfiguration(AUDIO_SOURCE_MISC_PARMS, open_param->stream_in_param.afe.misc_parms);
    AudioAfeConfiguration(AUDIO_SOURCE_ECHO_REFERENCE, echo_path);
#ifdef AIR_HFP_DNN_PATH_ENABLE
    AudioAfeConfiguration(AUDIO_SOURCE_DNN_PATH_ENABLE, open_param->stream_in_param.afe.enable_ul_dnn);
#endif

    AudioAfeConfiguration(AUDIO_SOURCE_UPDOWN_SAMPLER_ENABLE, open_param->stream_in_param.afe.with_upwdown_sampler);
    AudioAfeConfiguration(AUDIO_SOURCE_PATH_INPUT_RATE, open_param->stream_in_param.afe.audio_path_input_rate);
    AudioAfeConfiguration(AUDIO_SOURCE_PATH_OUTPUT_RATE, open_param->stream_in_param.afe.audio_path_output_rate);
    DSP_MW_LOG_I("afe in format:%d, sampling rate:%d, IRQ period:%d mise:0x%x\r\n", 4, open_param->stream_in_param.afe.format,
                                                                    gAudioCtrl.Afe.AfeULSetting.rate,
                                                                    open_param->stream_in_param.afe.irq_period,
                                                                    open_param->stream_in_param.afe.misc_parms);

    //TEMP!! should remove Audio_Default_setting_init
    Audio_setting->Audio_source.Buffer_Frame_Num = open_param->stream_in_param.afe.frame_number;
    Audio_setting->Audio_source.Frame_Size       = open_param->stream_in_param.afe.frame_size;

    if (open_param->stream_in_param.afe.memory&HAL_AUDIO_MEM_SUB) {
        //Sub-Source
        source = StreamAudioAfeSubSource(afe_get_audio_hardware_by_au_afe_open_param(open_param->stream_in_param.afe),
                                         afe_get_audio_instance_by_au_afe_open_param(open_param->stream_in_param.afe),
                                         afe_get_audio_channel_by_au_afe_open_param(open_param->stream_in_param.afe));
#ifdef MTK_TDM_ENABLE
    } else if (open_param->stream_in_param.afe.memory&HAL_AUDIO_MEM7) {
        //Tdm-Source
        source = StreamAudioAfeTdmSource(afe_get_audio_hardware_by_au_afe_open_param(open_param->stream_in_param.afe),
                                      afe_get_audio_instance_by_au_afe_open_param(open_param->stream_in_param.afe),
                                      afe_get_audio_channel_by_au_afe_open_param(open_param->stream_in_param.afe));
#endif
#ifdef AIR_I2S_SLAVE_ENABLE
    } else if(open_param->stream_in_param.afe.memory&HAL_AUDIO_MEM2){
        DSP_MW_LOG_I("source enter HAL_AUDIO_MEM2",0);
        source = StreamAudioAfe2Source(afe_get_audio_hardware_by_au_afe_open_param(open_param->stream_in_param.afe),
                                      afe_get_audio_instance_by_au_afe_open_param(open_param->stream_in_param.afe),
                                      afe_get_audio_channel_by_au_afe_open_param(open_param->stream_in_param.afe));
#endif
    } else {
        source = StreamAudioAfeSource(afe_get_audio_hardware_by_au_afe_open_param(open_param->stream_in_param.afe),
                                      afe_get_audio_instance_by_au_afe_open_param(open_param->stream_in_param.afe),
                                      afe_get_audio_channel_by_au_afe_open_param(open_param->stream_in_param.afe));
    }

    if (source != NULL) {

    } else {
        DSP_MW_LOG_E("DSP source create fail\r\n", 0);
    }
    return source;

}
#endif /* AIR_AUDIO_HARDWARE_ENABLE */

#ifdef MTK_BT_HFP_ENABLE
CONNECTION_IF n9_sco_dl_if;

SOURCE dsp_open_stream_in_hfp(mcu2dsp_open_param_p open_param)
{
    SOURCE source;
    DSP_MW_LOG_I("Stream in hfp\r\n", 0);
    DSP_MW_LOG_I("hfp in Codec:%d\r\n", 1, open_param->stream_in_param.hfp.codec_type);

    if (open_param->stream_in_param.hfp.codec_type == BT_HFP_CODEC_CVSD) {
        DSP_ALG_UpdateEscoRxMode(CVSD);
        stream_feature_configure_type(stream_feature_list_hfp_downlink, CODEC_DECODER_CVSD, CONFIG_DECODER);
    }
    else if (open_param->stream_in_param.hfp.codec_type  == BT_HFP_CODEC_mSBC) {
        DSP_ALG_UpdateEscoRxMode(mSBC);
        stream_feature_configure_type(stream_feature_list_hfp_downlink, CODEC_DECODER_MSBC, CONFIG_DECODER);
    }
    else {
        //not support codec type
    }
    n9_sco_dl_if.pfeature_table = (stream_feature_list_ptr_t)&stream_feature_list_hfp_downlink;

    DSP_Callback_PreloaderConfig(n9_sco_dl_if.pfeature_table);


    source = StreamN9ScoSource(open_param->stream_in_param.hfp.p_share_info);
    if (source != NULL) {
        source->streamBuffer.AVMBufferInfo.SampleRate = 16000;
        source->param.n9sco.share_info_base_addr->SampleRate = 16000;
        AudioAfeConfiguration(AUDIO_SINK_IRQ_RATE, 16000);
    } else {
        DSP_MW_LOG_E("DSP source create fail\r\n", 0);
    }

    /* rcdc clk skew */
    #ifndef AIR_BT_CLK_SKEW_ENABLE
    Clock_Skew_DL_Para_Init();
    #endif
    #ifndef MTK_BT_HFP_SPE_ALG_V2
    rcdc_clk_info_ptr = (RCDC_BT_CLK_INFO_t *)open_param->stream_in_param.hfp.clk_info_address;
    rcdc_clk_offset_info_ptr = (RCDC_CLK_OFFSET_INFO_t *)open_param->stream_in_param.hfp.bt_inf_address;
    #endif

    #ifdef MTK_AIRDUMP_EN
    rAirDumpCtrl = (AIRDUMPCTRL_t *)open_param->stream_in_param.hfp.p_air_dump_buf;
    #endif

    return source;
}
#endif

#ifdef AIR_BT_CODEC_BLE_ENABLED
EXTERN void lc3_dual_decode_mode_set(BOOL isDualMode);
LC3I_DL_PIC_Type_T lc3_dl_pic_type;
LC3I_UL_PIC_Type_T lc3_ul_pic_type;
SOURCE dsp_open_stream_in_ble(mcu2dsp_open_param_p open_param)
{
    SOURCE source;
    DSP_MW_LOG_I("[BLE] Stream in ble, Codec:%d", 1, open_param->stream_in_param.ble.codec_type);

    switch(open_param->stream_in_param.ble.context_type){

        case BLE_CONTEXT_CONVERSATIONAL:
            n9_ble_dl_if.pfeature_table = AudioFeatureList_BLE_Call_DL;
            break;

        default:
            n9_ble_dl_if.pfeature_table = AudioFeatureList_BLE_Music_DL;
            break;
    }

    if(open_param->stream_in_param.ble.channel_mode == CHANNEL_MODE_DL_ONLY){
        n9_ble_dl_if.pfeature_table = AudioFeatureList_BLE_Music_DL;
    }

    DSP_ALG_UpdateEscoTxMode(mSBC);         /*16K sample rate. Need WB algorithm*/
    DSP_ALG_UpdateEscoRxMode(mSBC);         /*16K sample rate. Need WB algorithm*/

    if (open_param->stream_in_param.ble.codec_type == BT_BLE_CODEC_LC3) {
        stream_feature_configure_type(n9_ble_dl_if.pfeature_table, CODEC_DECODER_LC3, CONFIG_DECODER);
        lc3_dual_decode_mode_set(open_param->stream_in_param.ble.channel_num == 2);

        switch(open_param->stream_in_param.ble.context_type){
            case BLE_CONTEXT_CONVERSATIONAL:
                lc3i_plc_std_library_load();

                if(open_param->stream_in_param.ble.frame_ms == 10000){
                    lc3i_fft10ms_library_load();
                    lc3_dl_pic_type = LC3I_DL_CALL_10_MS;
                }else if(open_param->stream_in_param.ble.frame_ms == 7500){
                        lc3i_fft7_5ms_library_load();
                        lc3_dl_pic_type = LC3I_DL_CALL_7P5_MS;
                }else{
                        DSP_MW_LOG_E("[BLE] unsupported DL frame interval %d",1,open_param->stream_in_param.ble.frame_ms);
                }
                break;

            default:
                lc3i_plc_adv_library_load();
                if(open_param->stream_in_param.ble.frame_ms == 10000){
                    lc3i_fft10ms_library_load();
                    lc3_dl_pic_type = LC3I_DL_AUDIO_10_MS;
                }else if(open_param->stream_in_param.ble.frame_ms == 7500){
                    lc3i_fft7_5ms_library_load();
                    lc3_dl_pic_type = LC3I_DL_AUDIO_7P5_MS;
               }else{
                    DSP_MW_LOG_E("[BLE] unsupported DL frame interval %d",1,open_param->stream_in_param.ble.frame_ms);
               }
               break;
        }
    }


    DSP_Callback_PreloaderConfig(n9_ble_dl_if.pfeature_table);

    source = StreamN9BleSource(open_param);

    return source;
}
#endif

#ifdef MTK_BT_A2DP_ENABLE
SOURCE dsp_open_stream_in_a2dp(mcu2dsp_open_param_p open_param)
{
    SOURCE source;
    U32 samplerate = 0, channel = 0;

    DSP_MW_LOG_I("Stream In A2DP\r\n", 0);
    source  = StreamN9A2dpSource(&open_param->stream_in_param.a2dp);

    if (source != NULL) {
        /* parse codec info */
        samplerate  = a2dp_get_samplingrate(source);
        channel     = a2dp_get_channel(source);

        DSP_MW_LOG_I("A2DP codec type: %d, sr: %d, SL: %d\r\n", 3, source->param.n9_a2dp.codec_info.codec_cap.type, samplerate, source->param.n9_a2dp.sink_latency);
        n9_a2dp_if.pfeature_table = stream_feature_list_a2dp;
        if ( source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR ) {
            #ifdef MTK_BT_A2DP_ENABLE
            DSP_UpdateA2DPCodec(BT_A2DP_CODEC_VENDOR);
            #endif
            DSP_MW_LOG_I("A2DP codec :%d", 1,source->param.n9_a2dp.codec_info.codec_cap.type);
            #if !defined(MTK_BT_A2DP_VENDOR_ENABLE) && !defined(MTK_BT_A2DP_VENDOR_1_ENABLE)
            DSP_MW_LOG_E("A2DP request unsupported codec", 0);
            configASSERT(0);
            #endif
            #ifdef MTK_BT_A2DP_VENDOR_BC_ENABLE
            if (source->param.n9_a2dp.sink_latency == 0)
            {
                n9_a2dp_if.pfeature_table = stream_feature_list_vend_a2dp;
            }
            #endif
            #ifdef MTK_BT_A2DP_VENDOR_1_ENABLE
            stream_feature_configure_type(n9_a2dp_if.pfeature_table, CODEC_DECODER_VENDOR_1, CONFIG_DECODER);
            #else
            stream_feature_configure_type(n9_a2dp_if.pfeature_table, CODEC_DECODER_VENDOR, CONFIG_DECODER);
            #endif
            //not support codec type
            #ifdef MTK_AUDIO_PLC_ENABLE
            Audio_PLC_ctrl(open_param->stream_in_param.a2dp.audio_plc);
            #endif
        }
        else if ( source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_SBC ) {
            #ifdef MTK_BT_A2DP_ENABLE
            DSP_UpdateA2DPCodec(BT_A2DP_CODEC_SBC);
            #endif
            stream_feature_configure_type(n9_a2dp_if.pfeature_table, CODEC_DECODER_SBC, CONFIG_DECODER);
            #ifdef MTK_AUDIO_PLC_ENABLE
            Audio_PLC_ctrl(open_param->stream_in_param.a2dp.audio_plc);
            #endif
        }
        else if ( source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_AAC ) {
            #ifdef MTK_BT_A2DP_ENABLE
            DSP_UpdateA2DPCodec(BT_A2DP_CODEC_AAC);
            #endif
            stream_feature_configure_type(n9_a2dp_if.pfeature_table, CODEC_DECODER_AAC, CONFIG_DECODER);
            #ifdef MTK_AUDIO_PLC_ENABLE
            Audio_PLC_ctrl(open_param->stream_in_param.a2dp.audio_plc);
            #endif
        }
        #ifdef MTK_GAMING_MODE_HEADSET
        else if ( source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_AIRO_CELT ) {
            samplerate = 48000;
            #ifdef MTK_BT_A2DP_ENABLE
            DSP_UpdateA2DPCodec(BT_A2DP_CODEC_AIRO_CELT);
            #endif
            stream_feature_configure_type(n9_a2dp_if.pfeature_table, CODEC_DECODER_CELT_HD, CONFIG_DECODER);
        }
        #endif

        source->streamBuffer.AVMBufferInfo.SampleRate = samplerate;
        memcpy(&(((SHARE_BUFFER_INFO_PTR)(source->param.n9_a2dp.share_info_base_addr))->sample_rate), &(source->streamBuffer.AVMBufferInfo.SampleRate), 4);/* update sample_rate */
        #ifdef AIR_AUDIO_HARDWARE_ENABLE
        AudioAfeConfiguration(AUDIO_SINK_IRQ_RATE, samplerate);
        #endif /* AIR_AUDIO_HARDWARE_ENABLE */

        #ifdef AIR_BT_CLK_SKEW_ENABLE /* long-term clock skew */
        Clock_Skew_DL_Para_Init();
        lt_clk_skew_reset_info();
        lt_clk_skew_set_sample_rate(samplerate);
        lt_clk_skew_set_asi_buf(source->param.n9_a2dp.asi_buf);
        lt_clk_skew_set_min_gap_buf(source->param.n9_a2dp.min_gap_buf);
        lt_clk_skew_set_sink_latency(source->param.n9_a2dp.sink_latency);
        #endif

        #ifndef MTK_BT_HFP_SPE_ALG_V2
        /* rcdc clock skew */
        //rcdc_clk_info_ptr = (RCDC_BT_CLK_INFO_t *)open_param->stream_in_param.a2dp.clk_info_address;
        rcdc_clk_offset_info_ptr = (RCDC_CLK_OFFSET_INFO_t *)open_param->stream_in_param.a2dp.bt_inf_address;
        #endif
        source->param.n9_a2dp.readOffset = open_param->stream_in_param.a2dp.p_afe_buf_report;// store afe buffer report instead
        #ifdef MTK_PEQ_ENABLE
        PEQ_Reset_Info();
        #endif

        DSP_Callback_PreloaderConfig((stream_feature_list_ptr_t)n9_a2dp_if.pfeature_table);
    } else {
        DSP_MW_LOG_E("DSP source create fail\r\n", 0);
    }
    return source;
}
#endif

SOURCE dsp_open_stream_in_playback(mcu2dsp_open_param_p open_param)
{
    SOURCE source;
    U32 sample_rate;
    DSP_MW_LOG_I("Stream in playback\r\n", 0);

    source = StreamCM4PlaybackSource(&open_param->stream_in_param.playback);

    if (source) {
        sample_rate = source->param.cm4_playback.info.sampling_rate;

        /* Yo: Should switch to VP afe sink later */
        source->streamBuffer.ShareBufferInfo.sample_rate = sample_rate;
        #ifdef AIR_AUDIO_HARDWARE_ENABLE
        AudioAfeConfiguration(AUDIO_SINK_IRQ_RATE, sample_rate);
        #endif /* AIR_AUDIO_HARDWARE_ENABLE */
        playback_if.pfeature_table =  (stream_feature_list_ptr_t)&stream_feature_list_playback;
        stream_feature_configure_type(playback_if.pfeature_table, CODEC_PCM_COPY, CONFIG_DECODER);
    } else {
        DSP_MW_LOG_E("DSP source create fail\r\n", 0);
    }

    return source;
}

SOURCE dsp_open_stream_in_vp(mcu2dsp_open_param_p open_param)
{
    SOURCE source = NULL;
    U32 sample_rate;
    DSP_MW_LOG_I("Stream in vp\r\n", 0);
#ifdef MTK_PROMPT_SOUND_ENABLE
    source = StreamCM4VPPlaybackSource(&open_param->stream_in_param.playback);
#else
    UNUSED(open_param);
#endif
    if (source) {
        sample_rate = source->param.cm4_playback.info.sampling_rate;

        /* Yo: Should switch to VP afe sink later */
        source->streamBuffer.ShareBufferInfo.sample_rate = sample_rate;
        #ifdef AIR_AUDIO_HARDWARE_ENABLE
        AudioAfeConfiguration(AUDIO_SINK_IRQ_RATE, sample_rate);
        #endif /* AIR_AUDIO_HARDWARE_ENABLE */

        stream_feature_configure_type(stream_feature_list_prompt, CODEC_PCM_COPY, CONFIG_DECODER);
    } else {
        DSP_MW_LOG_E("DSP source create fail\r\n", 0);
    }

    return source;
}

#ifdef AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
SOURCE dsp_open_stream_in_vp_dummy_source(mcu2dsp_open_param_p open_param)
{
    SOURCE source;
    U32 sample_rate;
    DSP_MW_LOG_I("Stream in VP Dummy Source\r\n", 0);

    source = StreamCM4VPDummySourcePlaybackSource(&open_param->stream_in_param.playback);

    if (source) {
        sample_rate = source->param.cm4_playback.info.sampling_rate;

        /* Should switch to VP afe sink later */
        source->streamBuffer.ShareBufferInfo.sample_rate = sample_rate;
        AudioAfeConfiguration(AUDIO_SINK_IRQ_RATE, sample_rate);

        stream_feature_configure_type(stream_feature_list_prompt_dummy_source, CODEC_PCM_COPY, CONFIG_DECODER);
    } else {
        DSP_MW_LOG_I("DSP source create fail\r\n", 0);
    }

    return source;
}
#endif

#ifdef MTK_SENSOR_SOURCE_ENABLE
SOURCE dsp_open_stream_in_gsensor(mcu2dsp_open_param_p open_param)
{
    SOURCE source;

    open_param = open_param;

    source = StreamGsensorSource();

    return source;
}
#endif

#ifdef MTK_AUDIO_TRANSMITTER_ENABLE
SOURCE dsp_open_stream_in_audio_transmitter(mcu2dsp_open_param_p open_param)
{
    SOURCE source;

    DSP_MW_LOG_I("Stream in aduio transmitter\r\n", 0);

    source = StreamAudioTransmitterSource(&(open_param->stream_in_param.data_dl));
    if (source)
    {
        DSP_MW_LOG_I("DSP source create successfully\r\n", 0);
    }
    else
    {
        DSP_MW_LOG_E("DSP source create fail\r\n", 0);
    }

    return source;
}
#endif /* MTK_AUDIO_TRANSMITTER_ENABLE */

#ifdef MTK_AUDIO_BT_COMMON_ENABLE
SOURCE dsp_open_stream_in_bt_common(mcu2dsp_open_param_p open_param)
{
    SOURCE source;

    DSP_MW_LOG_I("Stream in bt common\r\n", 0);

    source = StreamBTCommonSource(&(open_param->stream_in_param.bt_dl));

    if (source)
    {
        DSP_MW_LOG_I("DSP source create successfully\r\n", 0);
    }
    else
    {
        DSP_MW_LOG_E("DSP source create fail\r\n", 0);
    }

    return source;
}
#endif /* MTK_AUDIO_BT_COMMON_ENABLE */

SOURCE dsp_open_stream_in(mcu2dsp_open_param_p open_param)
{
    SOURCE source = NULL;
    if (open_param != NULL) {
        switch (open_param->param.stream_in) {
#ifdef AIR_AUDIO_HARDWARE_ENABLE
            case STREAM_IN_AFE:
                source = dsp_open_stream_in_afe(open_param);
                break;
#endif /* AIR_AUDIO_HARDWARE_ENABLE */
#ifdef MTK_BT_HFP_ENABLE
            case STREAM_IN_HFP:
                source = dsp_open_stream_in_hfp(open_param);
                break;
#endif
#ifdef AIR_BT_CODEC_BLE_ENABLED
            case STREAM_IN_BLE:
                source = dsp_open_stream_in_ble(open_param);
                break;
#endif
#ifdef MTK_BT_A2DP_ENABLE
            case STREAM_IN_A2DP:
                source = dsp_open_stream_in_a2dp(open_param);
                break;
#endif
            case STREAM_IN_PLAYBACK:
                source = dsp_open_stream_in_playback(open_param);
                break;
            case STREAM_IN_VP:
                source = dsp_open_stream_in_vp(open_param);
                break;
#ifdef AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
            case STREAM_IN_VP_DUMMY_SOURCE:
                source = dsp_open_stream_in_vp_dummy_source(open_param);
                break;
#endif
            #ifdef MTK_SENSOR_SOURCE_ENABLE
            case STREAM_IN_GSENSOR:
                source = dsp_open_stream_in_gsensor(open_param);
                break;
            #endif
            #ifdef MTK_AUDIO_TRANSMITTER_ENABLE
            case STREAM_IN_AUDIO_TRANSMITTER:
                source = dsp_open_stream_in_audio_transmitter(open_param);
                break;
            #endif /* MTK_AUDIO_TRANSMITTER_ENABLE */
            #ifdef MTK_AUDIO_BT_COMMON_ENABLE
            case STREAM_IN_BT_COMMON:
                source = dsp_open_stream_in_bt_common(open_param);
                break;
            #endif /* MTK_AUDIO_BT_COMMON_ENABLE */
            default:
                break;
        }
    }
    return source;
}

#ifdef MTK_BT_HFP_ENABLE
CONNECTION_IF n9_sco_ul_if;

SINK dsp_open_stream_out_hfp(mcu2dsp_open_param_p open_param)
{
    SINK sink;
    //DSP_MW_LOG_I("Stream out hfp\r\n", 0);
    //DSP_MW_LOG_I("hfp out Codec:%d\r\n", 1, open_param->stream_out_param.hfp.codec_type);

    if (open_param->stream_out_param.hfp.codec_type == BT_HFP_CODEC_CVSD ) {
        DSP_ALG_UpdateEscoTxMode(CVSD);
        stream_feature_configure_type(stream_feature_list_hfp_uplink, CODEC_ENCODER_CVSD, CONFIG_ENCODER);
    }
    else if (open_param->stream_out_param.hfp.codec_type  == BT_HFP_CODEC_mSBC ) {
        DSP_ALG_UpdateEscoTxMode(mSBC);
        stream_feature_configure_type(stream_feature_list_hfp_uplink, CODEC_ENCODER_MSBC, CONFIG_ENCODER);
    }
    else {
        //not support codec type
    }
    n9_sco_ul_if.pfeature_table = (stream_feature_list_ptr_t)&stream_feature_list_hfp_uplink;

    DSP_Callback_PreloaderConfig(n9_sco_ul_if.pfeature_table);

    sink = StreamN9ScoSink(open_param->stream_out_param.hfp.p_share_info);
    if (sink != NULL) {

    } else {
        DSP_MW_LOG_E("DSP sink create fail\r\n", 0);
    }

    return sink;
}
#endif

#ifdef AIR_BT_CODEC_BLE_ENABLED
SINK dsp_open_stream_out_ble(mcu2dsp_open_param_p open_param)
{
    SINK sink;

    DSP_MW_LOG_I("[BLE] Stream out ble, Codec:%d", 1, open_param->stream_out_param.ble.codec_type);

    /*  Tx Rx mode must be aligned otherwise it would cause NVkey assert*/
    /*  Currently Rx mode dominates the initialization process*/
    DSP_ALG_UpdateEscoTxMode(mSBC);         /*16K sample rate. Need WB algorithm*/
    DSP_ALG_UpdateEscoRxMode(mSBC);         /*16K sample rate. Need WB algorithm*/

    n9_ble_ul_if.pfeature_table = AudioFeatureList_BLE_Call_UL;
    if (open_param->stream_out_param.ble.codec_type == BT_BLE_CODEC_LC3) {
        lc3i_plc_std_library_load();
        if(open_param->stream_out_param.ble.frame_ms == 10000){
            lc3i_fft10ms_library_load();
            lc3_ul_pic_type = LC3I_UL_CALL_10_MS;

        }else if(open_param->stream_out_param.ble.frame_ms == 7500){
            lc3i_fft7_5ms_library_load();
            lc3_ul_pic_type = LC3I_UL_CALL_7P5_MS;
        }else{
            DSP_MW_LOG_E("[BLE] unsupported UL frame interval %d",1,open_param->stream_out_param.ble.frame_ms);
        }
    }

    DSP_Callback_PreloaderConfig(n9_ble_ul_if.pfeature_table);

    sink = StreamN9BleSink(open_param);

    return sink;
}
#endif

#ifdef AIR_AUDIO_HARDWARE_ENABLE
SINK dsp_open_stream_out_afe(mcu2dsp_open_param_p open_param)
{
    SINK sink = NULL;
    #ifdef AIR_DONGLE_AFE_USAGE_CHECK_ENABLE
    DSP_MW_LOG_I("AFE USAGE CHECK, AFE out exist", 0);
    configASSERT(0);
    #endif

    //[TEMP]: Add AT Cmd to switch I2S mode
    if(((*((volatile uint32_t*)(0xA2120B04)) >> 2)&0x01) == 1) {
        open_param->stream_out_param.afe.audio_device = HAL_AUDIO_CONTROL_DEVICE_SPDIF;//HAL_AUDIO_DEVICE_I2S_MASTER;
        open_param->stream_out_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
    }

    //DSP_MW_LOG_I("Stream out afe\r\n", 1);
    #ifdef ENABLE_HWSRC_CLKSKEW
    DSP_MW_LOG_I("afe out device:%d, channel:%d, memory:%d, interface:%d, hw_gain:%d, adc_mode:%d, performance:%d,clkskew_mode:%d \r\n", 8, open_param->stream_out_param.afe.audio_device,
                                                                         open_param->stream_out_param.afe.stream_channel,
                                                                         open_param->stream_out_param.afe.memory,
                                                                         open_param->stream_out_param.afe.audio_interface,
                                                                         open_param->stream_out_param.afe.hw_gain,
                                                                         open_param->stream_out_param.afe.dl_dac_mode,
                                                                         open_param->stream_out_param.afe.performance,
                                                                         open_param->stream_out_param.afe.clkskew_mode);
    #else
    DSP_MW_LOG_I("afe out device:%d, channel:%d, memory:%d, interface:%d, hw_gain:%d, adc_mode:%d, performance:%d \r\n", 8, open_param->stream_out_param.afe.audio_device,
                                                                         open_param->stream_out_param.afe.stream_channel,
                                                                         open_param->stream_out_param.afe.memory,
                                                                         open_param->stream_out_param.afe.audio_interface,
                                                                         open_param->stream_out_param.afe.hw_gain,
                                                                         open_param->stream_out_param.afe.adc_mode,
                                                                         open_param->stream_out_param.afe.performance);
    #endif

    AudioAfeConfiguration(AUDIO_SINK_DATA_FORMAT, open_param->stream_out_param.afe.format);
    if (open_param->stream_out_param.afe.sampling_rate != 0) {
        AudioAfeConfiguration(AUDIO_SINK_IRQ_RATE, open_param->stream_out_param.afe.sampling_rate);
    }

#ifdef ENABLE_HWSRC_ON_MAIN_STREAM
    AudioAfeConfiguration(AUDIO_SRC_RATE,open_param->stream_out_param.afe.stream_out_sampling_rate);
#endif
    AudioAfeConfiguration(AUDIO_SINK_IRQ_PERIOD, open_param->stream_out_param.afe.irq_period);

    AudioAfeConfiguration(AUDIO_SINK_DEVICE, open_param->stream_out_param.afe.audio_device);
    AudioAfeConfiguration(AUDIO_SINK_CHANNEL, open_param->stream_out_param.afe.stream_channel);
    AudioAfeConfiguration(AUDIO_SINK_MEMORY, open_param->stream_out_param.afe.memory);
    AudioAfeConfiguration(AUDIO_SINK_INTERFACE, open_param->stream_out_param.afe.audio_interface);
    AudioAfeConfiguration(AUDIO_SINK_HW_GAIN, open_param->stream_out_param.afe.hw_gain);
#ifdef ENABLE_HWSRC_CLKSKEW
    if (open_param->stream_out_param.afe.memory == HAL_AUDIO_MEM1) {
        if(ClkSkewMode_isModify_g == TRUE)
        {
           AudioAfeConfiguration(AUDIO_SINK_CLKSKEW_MODE, ClkSkewMode_g);
        }else{
           AudioAfeConfiguration(AUDIO_SINK_CLKSKEW_MODE, open_param->stream_out_param.afe.clkskew_mode);
           ClkSkewMode_g = open_param->stream_out_param.afe.clkskew_mode;
        }
        DSP_MW_LOG_I("afe clkskew_mode:%d %d", 2, open_param->stream_out_param.afe.clkskew_mode, ClkSkewMode_g);
    }
#endif
    #ifdef HAL_AUDIO_ENABLE_PATH_MEM_DEVICE
    AudioAfeConfiguration(AUDIO_SINK_ADC_MODE, open_param->stream_out_param.afe.dl_dac_mode);
    if (open_param->stream_out_param.afe.audio_device & (HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER | HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_L | HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_R)) {
        AudioAfeConfiguration(AUDIO_SINK_I2S_FORMAT, open_param->stream_out_param.afe.i2s_format);
        AudioAfeConfiguration(AUDIO_SINK_I2S_WORD_LENGTH, open_param->stream_out_param.afe.i2s_word_length);
    } else if (open_param->stream_out_param.afe.audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        AudioAfeConfiguration(AUDIO_SINK_I2S_FORMAT, open_param->stream_out_param.afe.i2s_format);
        AudioAfeConfiguration(AUDIO_SINK_I2S_SLAVE_TDM, open_param->stream_out_param.afe.i2S_Slave_TDM);
        AudioAfeConfiguration(AUDIO_SINK_I2S_WORD_LENGTH, open_param->stream_out_param.afe.i2s_word_length);
    }
    AudioAfeConfiguration(AUDIO_SINK_I2S_LOW_JITTER,open_param->stream_out_param.afe.is_low_jitter);
    AudioAfeConfiguration(AUDIO_SINK_DAC_PERFORMANCE,open_param->stream_out_param.afe.performance);
    #endif
    #if AUTO_ERROR_SUPPRESSION
    AudioAfeConfiguration(AUDIO_SINK_MISC_PARMS_I2S_CLK, open_param->stream_out_param.afe.misc_parms.I2sClkSourceType);
    AudioAfeConfiguration(AUDIO_SINK_MISC_PARMS_MICBIAS, open_param->stream_out_param.afe.misc_parms.MicbiasSourceType);
    #endif
    AudioAfeConfiguration(AUDIO_SINK_ECHO_REFERENCE, false);
#ifdef AIR_HFP_DNN_PATH_ENABLE
    AudioAfeConfiguration(AUDIO_SINK_DNN_PATH_ENABLE, open_param->stream_out_param.afe.enable_ul_dnn);
#endif
#ifdef AIR_WIRED_AUDIO_ENABLE
    AudioAfeConfiguration(AUDIO_SINK_UPDOWN_SAMPLER_ENABLE, open_param->stream_out_param.afe.with_upwdown_sampler);
    AudioAfeConfiguration(AUDIO_SINK_PATH_INPUT_RATE, open_param->stream_out_param.afe.audio_path_input_rate);
    AudioAfeConfiguration(AUDIO_SINK_PATH_OUTPUT_RATE, open_param->stream_out_param.afe.audio_path_output_rate);
#endif
    DSP_MW_LOG_I("afe out format:%d, sampling rate:%d, IRQ period:%d\r\n", 3, open_param->stream_out_param.afe.format,
                                                                     gAudioCtrl.Afe.AfeDLSetting.rate,
                                                                     open_param->stream_out_param.afe.irq_period);


#ifdef MTK_TDM_ENABLE
    if (open_param->stream_out_param.afe.memory == HAL_AUDIO_MEM1 || open_param->stream_out_param.afe.memory == HAL_AUDIO_MEM6) {
#else
    if (open_param->stream_out_param.afe.memory == HAL_AUDIO_MEM1 || open_param->stream_out_param.afe.memory == HAL_AUDIO_MEM6 || open_param->stream_out_param.afe.memory == HAL_AUDIO_MEM7) {
#endif
        AudioAfeConfiguration(AUDIO_SINK_SW_CHANNELS, open_param->stream_out_param.afe.sw_channels);
        Audio_setting->Audio_sink.Buffer_Frame_Num = open_param->stream_out_param.afe.frame_number;
        Audio_setting->Audio_sink.Frame_Size       = open_param->stream_out_param.afe.frame_size;
        sink = StreamAudioAfeSink(afe_get_audio_hardware_by_au_afe_open_param(open_param->stream_out_param.afe),
                                  afe_get_audio_instance_by_au_afe_open_param(open_param->stream_out_param.afe),
                                  afe_get_audio_channel_by_au_afe_open_param(open_param->stream_out_param.afe));

    } else if (open_param->stream_out_param.afe.memory == HAL_AUDIO_MEM2) {
        Audio_setting->Audio_VP.Buffer_Frame_Num = open_param->stream_out_param.afe.frame_number;
        Audio_setting->Audio_VP.Frame_Size = open_param->stream_out_param.afe.frame_size;
        // VP/RT memory path DL2_data
        #ifdef MTK_PROMPT_SOUND_ENABLE
        sink = StreamAudioAfe2Sink(afe_get_audio_hardware_by_au_afe_open_param(open_param->stream_out_param.afe),
                                   afe_get_audio_instance_by_au_afe_open_param(open_param->stream_out_param.afe),
                                   afe_get_audio_channel_by_au_afe_open_param(open_param->stream_out_param.afe));
        #endif
#ifdef MTK_TDM_ENABLE
    } else if (open_param->stream_out_param.afe.memory == HAL_AUDIO_MEM7) {
       //Tdm-Sink
       AudioAfeConfiguration(AUDIO_SINK_SW_CHANNELS, open_param->stream_out_param.afe.sw_channels);
       Audio_setting->Audio_sink.Buffer_Frame_Num = open_param->stream_out_param.afe.frame_number;
       Audio_setting->Audio_sink.Frame_Size       = open_param->stream_out_param.afe.frame_size;
       sink = StreamAudioAfeTdmSink(afe_get_audio_hardware_by_au_afe_open_param(open_param->stream_out_param.afe),
                                 afe_get_audio_instance_by_au_afe_open_param(open_param->stream_out_param.afe),
                                 afe_get_audio_channel_by_au_afe_open_param(open_param->stream_out_param.afe));
#endif
    } else if (open_param->stream_out_param.afe.memory == HAL_AUDIO_MEM3) {
        #if defined (AIR_WIRED_AUDIO_ENABLE) || defined (AIR_ADVANCED_PASSTHROUGH_ENABLE) || defined (AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE) || defined(AIR_HFP_DNN_PATH_ENABLE)
        AudioAfeConfiguration(AUDIO_SINK_SW_CHANNELS, open_param->stream_out_param.afe.sw_channels);
        Audio_setting->Audio_sink.Buffer_Frame_Num = open_param->stream_out_param.afe.frame_number;
        Audio_setting->Audio_sink.Frame_Size       = open_param->stream_out_param.afe.frame_size;
        DSP_MW_LOG_I("Create DL3 afe sink, Buffer_Frame_Num:%d, Frame_Size:%d", 2, Audio_setting->Audio_sink.Buffer_Frame_Num, Audio_setting->Audio_sink.Frame_Size);
        sink = StreamAudioAfe3Sink(afe_get_audio_hardware_by_au_afe_open_param(open_param->stream_out_param.afe),
                                   afe_get_audio_instance_by_au_afe_open_param(open_param->stream_out_param.afe),
                                   afe_get_audio_channel_by_au_afe_open_param(open_param->stream_out_param.afe));
        #endif
    }
#if defined (AIR_WIRED_AUDIO_ENABLE) || defined (AIR_ADVANCED_PASSTHROUGH_ENABLE)
    else if (open_param->stream_out_param.afe.memory == HAL_AUDIO_MEM4) {
        AudioAfeConfiguration(AUDIO_SINK_SW_CHANNELS, open_param->stream_out_param.afe.sw_channels);
        Audio_setting->Audio_sink.Buffer_Frame_Num = open_param->stream_out_param.afe.frame_number;
        Audio_setting->Audio_sink.Frame_Size       = open_param->stream_out_param.afe.frame_size;
        sink = StreamAudioAfe12Sink(afe_get_audio_hardware_by_au_afe_open_param(open_param->stream_out_param.afe),
                                   afe_get_audio_instance_by_au_afe_open_param(open_param->stream_out_param.afe),
                                   afe_get_audio_channel_by_au_afe_open_param(open_param->stream_out_param.afe));
    }
#endif /* AIR_ADVANCED_PASSTHROUGH_ENABLE */

    if (sink != NULL) {

    } else {
        DSP_MW_LOG_E("DSP sink create fail\r\n", 0);
    }
    return sink;

}
#endif /* AIR_AUDIO_HARDWARE_ENABLE */

#ifdef MTK_CM4_RECORD_ENABLE
SINK dsp_open_stream_out_record(mcu2dsp_open_param_p open_param)
{
    SINK sink;
    DSP_MW_LOG_I("Stream out record\r\n", 0);

    sink = StreamCm4RecordSink(&(open_param->stream_out_param.record));
    if (sink != NULL) {

    } else {
        DSP_MW_LOG_E("DSP sink create fail\r\n", 0);
    }
    return sink;
}
#endif

#if ViturlStreamEnable
SINK dsp_open_stream_out_virtual(mcu2dsp_open_param_p open_param)
{
    SINK sink;
    DSP_MW_LOG_I("Stream out virtual\r\n", 0);
    UNUSED(open_param);
    sink = StreamVirtualSink(NULL, NULL);
    if (sink != NULL) {

    } else {
        DSP_MW_LOG_E("DSP sink create fail\r\n", 0);
    }
    return sink;
}
#endif

#ifdef MTK_AUDIO_TRANSMITTER_ENABLE
SINK dsp_open_stream_out_audio_transmitter(mcu2dsp_open_param_p open_param)
{
    SINK sink;

    sink = StreamAudioTransmitterSink(&(open_param->stream_out_param.data_ul));
    if (sink) {
        DSP_MW_LOG_I("[audio transmitter]: sink create successfully\r\n", 0);
    }
    else
    {
        DSP_MW_LOG_E("[audio transmitter]: sink create fail\r\n", 0);
    }

    return sink;
}
#endif /* MTK_AUDIO_TRANSMITTER_ENABLE */

#ifdef MTK_AUDIO_BT_COMMON_ENABLE
SINK dsp_open_stream_out_bt_common(mcu2dsp_open_param_p open_param)
{
    SINK sink;

    DSP_MW_LOG_I("Stream out bt common\r\n", 0);

    sink = StreamBTCommonSink(&(open_param->stream_out_param.bt_ul));

    if (sink)
    {
        DSP_MW_LOG_I("DSP sink create successfully\r\n", 0);
    }
    else
    {
        DSP_MW_LOG_E("DSP sink create fail\r\n", 0);
    }

    return sink;
}
#endif /* MTK_AUDIO_BT_COMMON_ENABLE */


SINK dsp_open_stream_out(mcu2dsp_open_param_p open_param)
{
    SINK sink = NULL;
    if (open_param != NULL) {
        switch (open_param->param.stream_out) {
#ifdef AIR_AUDIO_HARDWARE_ENABLE
            case STREAM_OUT_AFE:
                sink = dsp_open_stream_out_afe(open_param);
                break;
#endif /* AIR_AUDIO_HARDWARE_ENABLE */
#ifdef MTK_BT_HFP_ENABLE
            case STREAM_OUT_HFP:
                sink = dsp_open_stream_out_hfp(open_param);
                break;
#endif
#ifdef AIR_BT_CODEC_BLE_ENABLED
            case STREAM_OUT_BLE:
                sink = dsp_open_stream_out_ble(open_param);
                break;
#endif
#ifdef MTK_CM4_RECORD_ENABLE
            case STREAM_OUT_RECORD:
                sink = dsp_open_stream_out_record(open_param);
                break;
#endif
#ifdef MTK_AUDIO_TRANSMITTER_ENABLE
            case STREAM_OUT_AUDIO_TRANSMITTER:
                sink = dsp_open_stream_out_audio_transmitter(open_param);
                break;
#endif /* MTK_AUDIO_TRANSMITTER_ENABLE */
            case STREAM_OUT_VIRTUAL:
                #if ViturlStreamEnable
                sink = dsp_open_stream_out_virtual(open_param);
                #endif
                break;
            #ifdef MTK_AUDIO_BT_COMMON_ENABLE
            case STREAM_OUT_BT_COMMON:
                sink = dsp_open_stream_out_bt_common(open_param);
                break;
            #endif /* MTK_AUDIO_BT_COMMON_ENABLE */
            default:
                break;
        }
    }
    return sink;
}

#ifdef AIR_AUDIO_HARDWARE_ENABLE
void dsp_start_stream_in_afe(mcu2dsp_start_param_p start_param, SOURCE source)
{
    source->param.audio.AfeBlkControl.u4awsflag = start_param->stream_in_param.afe.mce_flag;
    DSP_MW_LOG_I("Stream in afe start MCE:%d\r\n", 1, source->param.audio.AfeBlkControl.u4awsflag);
}
#endif /* AIR_AUDIO_HARDWARE_ENABLE */


#ifdef PLAY_EN_CHECK_TIMER
#define CONN_BT_TIMCON_BASE 0xB0000000
static TimerHandle_t playen_check_timer = NULL;
static void playen_check_timer_callback(TimerHandle_t xTimer)
{
    UNUSED(xTimer);
    U32 playen_clk = *((volatile uint32_t *)(CONN_BT_TIMCON_BASE + 0x0204));
    U32 native_clk = rBb->rClkCtl.rNativeClock;
    if ((Sink_blks[SINK_TYPE_AUDIO] != NULL)&&(Sink_blks[SINK_TYPE_AUDIO]->param.audio.irq_exist == FALSE))
    {
        DSP_MW_LOG_I("Play en check, native bt_clk 0x%x :0x%x, play en  bt_clk 0x%x :0x%x, enable :0x%x", 5, native_clk, rBb->rClkCtl.rNativePhase, playen_clk,*((volatile uint32_t *)(CONN_BT_TIMCON_BASE + 0x0208)),*((volatile uint8_t *)(CONN_BT_TIMCON_BASE + 0x0200)));
        if ((n9_a2dp_if.transform != NULL)&&((native_clk > (playen_clk + 0x20)) && (native_clk  < 0xFFFFC00)))
        {
            DSP_MW_LOG_E("AFE wait play en trigger re-sync", 0);
            #ifdef MTK_BT_A2DP_ENABLE
            Au_DL_send_reinit_request(MSG2_DSP2CN4_REINIT_AFE_ABNORMAL);
            #endif
        }
    }
    else
    {
        xTimerStop(playen_check_timer, 0);
    }
}
#endif
U32 initial_asi = 0;
#ifdef MTK_BT_A2DP_ENABLE
#ifdef TRIGGER_A2DP_PROCSESS_TIMER
static TimerHandle_t trig_a2dp_proc_timer = NULL;

static void afe_prefill_for_src_out(U16 fill_sample_num,SINK sink)
{
    DSP_MW_LOG_E("prefill samples:%d,remain space:%d", 2,fill_sample_num,SinkSlack(sink));
    if ((fill_sample_num*sink->param.audio.format_bytes) > SinkSlack(sink))
    {
        return;
    }
    U32 buffer_per_channel_shift = ((sink->param.audio.channel_num>=2) && (sink->buftype == BUFFER_TYPE_INTERLEAVED_BUFFER ))
                             ? 1
                             : 0;
    U32 mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    SinkFlush(sink,fill_sample_num*sink->param.audio.format_bytes);
    sink->param.audio.sram_empty_fill_size = (fill_sample_num*sink->param.audio.format_bytes)<<buffer_per_channel_shift;
    hal_nvic_restore_interrupt_mask(mask);
}

static void trigger_a2dp_proc_timer_callback(TimerHandle_t xTimer)
{
    UNUSED(xTimer);

    if ((n9_a2dp_if.source != NULL)&&(n9_a2dp_if.sink != NULL)&&(n9_a2dp_if.transform != NULL)&&(n9_a2dp_if.sink->param.audio.irq_exist == FALSE))
    {
        DSP_MW_LOG_I("trigger_a2dp_proc_timer_callback start", 0);
        n9_a2dp_if.source->transform->Handler(n9_a2dp_if.source, n9_a2dp_if.sink);
        if (n9_a2dp_if.source->transform->sink->taskid == DPR_TASK_ID) {
            vTaskResume(pDPR_TaskHandler);
        } else if (n9_a2dp_if.source->transform->sink->taskid == DHP_TASK_ID) {
            vTaskResume(pDHP_TaskHandler);
        } else {
            vTaskResume(pDAV_TaskHandler);
        }
    } else {
        DSP_MW_LOG_I("trigger_a2dp_proc_timer_callback stop", 0);
        xTimerStop(trig_a2dp_proc_timer, 0);
    }
}
#endif

void dsp_start_stream_in_a2dp(mcu2dsp_start_param_p start_param, SOURCE source)
{
    source->param.n9_a2dp.cp_exist = FALSE;
    source->param.n9_a2dp.predict_asi = start_param->stream_in_param.a2dp.start_asi;
    source->param.n9_a2dp.latency_monitor = start_param->stream_in_param.a2dp.latency_monitor_enable;
    source->param.n9_a2dp.DspReportStartId = 0xFFFF;
    source->param.n9_a2dp.alc_monitor = !start_param->stream_in_param.a2dp.alc_monitor;// If Host already enabled ALC, DSP not to trigger ALC mode

    //source->param.n9_a2dp.latency_monitor = TRUE;
    U32 bt_clk;
    U16 intra_clk;
    #ifdef MTK_GAMING_MODE_HEADSET
    if (source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_AIRO_CELT) {
        /* ULL should wait for play en parameter from controller */
        if (audio_headset_ull_ctrl.is_play_en_ready) {
            audio_headset_ull_ctrl.play_en.bt_clock +=  ((source->param.n9_a2dp.sink_latency - ULL_BASIC_LATENCY)*8/2500);/* 8/2500 = 312.5 for frame based  */

            a2dp_ull_get_proper_play_en(audio_headset_ull_ctrl.play_en.bt_clock,
                                        audio_headset_ull_ctrl.play_en.sequence_number,
                                        &audio_headset_ull_ctrl.play_en.bt_clock,
                                        (U8*)&audio_headset_ull_ctrl.play_en.sequence_number);

            a2dp_ull_set_init_seq_no(audio_headset_ull_ctrl.play_en.sequence_number);

            a2dp_ull_drop_expired_pkt(source);

            /* AVM buffer needs to update SampleRate as 48k, this is temp */
            source->streamBuffer.AVMBufferInfo.SampleRate = 48000;
            if ((n9_a2dp_if.sink != NULL)&&(n9_a2dp_if.sink->type == SINK_TYPE_AUDIO))
            {
                n9_a2dp_if.sink->streamBuffer.BufferInfo.WriteOffset = 720;// temp modify prefill samples for ULL
            }
            MCE_TransBT2NativeClk(audio_headset_ull_ctrl.play_en.bt_clock, 0, &bt_clk, &intra_clk,ULL_CLK_Offset);
            #ifdef AIR_AUDIO_HARDWARE_ENABLE
            hal_audio_afe_set_play_en(bt_clk,intra_clk);
            #endif /* AIR_AUDIO_HARDWARE_ENABLE */
            DSP_MW_LOG_I("A2DP DL - A2DP Set Play En Clk:0x%x Intra:0x%x Modify AFE prefill offest to 720", 2, (uint32_t)bt_clk, (uint32_t)intra_clk);
        }
        audio_headset_ull_ctrl.is_a2dp_started = TRUE;
    } else
    #endif
    {
    MCE_GetBtClk((BTCLK *)&bt_clk,(BTPHASE *)&intra_clk,BT_CLK_Offset);
    DSP_MW_LOG_I("Get host set asi:0x%x b:0x%x i:0x%x",3,start_param->stream_in_param.a2dp.start_asi,start_param->stream_in_param.a2dp.start_bt_clk,start_param->stream_in_param.a2dp.start_bt_intra_clk);
    DSP_MW_LOG_I("Get local clk, n:0x%x b:0x%x i:0x%x",3,rBb->rClkCtl.rNativeClock,bt_clk,intra_clk);
    DSP_MW_LOG_I("DSP latency_monitor :%d ALC mode :%d", 2,start_param->stream_in_param.a2dp.latency_monitor_enable,start_param->stream_in_param.a2dp.alc_monitor);
    start_param->stream_in_param.a2dp.start_bt_intra_clk = (start_param->stream_in_param.a2dp.start_bt_intra_clk&0xFFFF)<<1;

    if (abs32((S32)(start_param->stream_in_param.a2dp.start_bt_clk - bt_clk)) >  0x2000) // Maximum tolerence time difference = 0x2000*312.5(us) = 2.56(s)
    {
        DSP_MW_LOG_I("Host notify play time abnormal, bt_clk: 0x%x play_clk: 0x%x",2,bt_clk, start_param->stream_in_param.a2dp.start_bt_clk);
        start_param->stream_in_param.a2dp.start_bt_clk = bt_clk + 0x100;
        //configASSERT(0);
    }
    initial_asi = source->param.n9_a2dp.predict_asi;
    if ((S32)(start_param->stream_in_param.a2dp.start_bt_clk - bt_clk) > 0x40)  // check play time in time
    {
        DSP_MW_LOG_I("Set legal play en",0);
    }
    else
    {
        U32 temp_start_bt_clk = start_param->stream_in_param.a2dp.start_bt_clk;
        while ((S32)(temp_start_bt_clk - bt_clk) < 0x40)
        {
            source->param.n9_a2dp.predict_asi += 1024;
            temp_start_bt_clk +=  3276800 / (source->streamBuffer.AVMBufferInfo.SampleRate); // roughly calc
        }
        start_param->stream_in_param.a2dp.start_bt_clk +=((((source->param.n9_a2dp.predict_asi - start_param->stream_in_param.a2dp.start_asi)<<5)*100 / (source->streamBuffer.AVMBufferInfo.SampleRate))& 0xFFFFFFC);
        start_param->stream_in_param.a2dp.start_bt_intra_clk += ((((source->param.n9_a2dp.predict_asi - start_param->stream_in_param.a2dp.start_asi))*20000)/(source->streamBuffer.AVMBufferInfo.SampleRate/100))%2500;
        DSP_MW_LOG_I("Host notify play time too early, modify play en b:0x%x i:0x%x",2,start_param->stream_in_param.a2dp.start_bt_clk,start_param->stream_in_param.a2dp.start_bt_intra_clk);
    }
    MCE_TransBT2NativeClk(start_param->stream_in_param.a2dp.start_bt_clk,start_param->stream_in_param.a2dp.start_bt_intra_clk,&bt_clk,&intra_clk,BT_CLK_Offset);
    DSP_MW_LOG_I("Play en b:0x%x i:0x%x n:0x%x asi: 0x%x",4,bt_clk,intra_clk,rBb->rClkCtl.rNativeClock,source->param.n9_a2dp.predict_asi);
    #ifdef AIR_AUDIO_HARDWARE_ENABLE
    hal_audio_afe_set_play_en(bt_clk,intra_clk);
    #endif /* AIR_AUDIO_HARDWARE_ENABLE */
    #ifdef TRIGGER_A2DP_PROCSESS_TIMER
    U32 bt_clk_cur;
    U16 intra_clk_cur;
    MCE_GetBtClk((BTCLK *)&bt_clk_cur,(BTPHASE *)&intra_clk_cur,BT_CLK_Offset);
    U32 bt_clk_duration = start_param->stream_in_param.a2dp.start_bt_clk-bt_clk_cur;
    U32 duration_ms =  ((bt_clk_duration>>1)*625)/1000;
    DSP_MW_LOG_I("trigger_a2dp_proc_timer, bclk_play_en:%d, bclk_cur:%d, timer_duration:%d ms", 3, start_param->stream_in_param.a2dp.start_bt_clk, bt_clk_cur, duration_ms);
    if(duration_ms > 10) { /* the tick unit of DSP is 10ms*/
        if (trig_a2dp_proc_timer == NULL)
        {
            trig_a2dp_proc_timer = xTimerCreate("TRIGGER_A2DP_PROCSESS_TIMER", pdMS_TO_TICKS(duration_ms), pdFALSE, 0, trigger_a2dp_proc_timer_callback);
        }
        if(!trig_a2dp_proc_timer) {
            DSP_MW_LOG_I("trigger_a2dp_proc_timer create timer FAIL", 0);
        } else {
            DSP_MW_LOG_I("trigger_a2dp_proc_timer create timer PASS", 0);
            xTimerStart(trig_a2dp_proc_timer, 0);
        }
	}
    //else if(SourceSize(source) == 0)
    {
        DSP_MW_LOG_E("Add prefill to prevent SRC out underrun on a2dp start", 0);
        afe_prefill_for_src_out(((2*(U32)(n9_a2dp_if.sink->param.audio.period)*(n9_a2dp_if.sink->param.audio.rate/1000)+256)&(0xFF00)),n9_a2dp_if.sink);
    }
    #endif /*TRIGGER_A2DP_PROCSESS_TIMER*/
    #ifdef PLAY_EN_CHECK_TIMER
    if (playen_check_timer == NULL)
    {
        playen_check_timer = xTimerCreate("PLANEN_CHECK_TIMER", pdMS_TO_TICKS(30), pdTRUE, 0, playen_check_timer_callback);
    }
    if(!playen_check_timer) {
        DSP_MW_LOG_I("playen_check_timer create timer FAIL", 0);
    } else {
        DSP_MW_LOG_I("playen_check_timer create timer PASS", 0);
        xTimerChangePeriod(playen_check_timer, pdMS_TO_TICKS(30), 0);
    }
    #endif
	}
    MCE_Initial_Aud_Cnt_from_Controller();
}
#endif

#ifdef MTK_AUDIO_TRANSMITTER_ENABLE
void dsp_start_stream_in_audio_transmitter(mcu2dsp_start_param_p start_param, SOURCE source)
{
    UNUSED(start_param);

    DSP_MW_LOG_I("Stream in audio transmitter start:%d, %d\r\n", 2, source->param.data_dl.scenario_type, source->param.data_dl.scenario_sub_id);

    switch(source->param.data_dl.scenario_type)
    {
        #if defined(MTK_GAMING_MODE_HEADSET) || defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) ||
                (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1))
            {
                /* start the first timer */
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.timer.timestamp);
                // hal_gpt_sw_start_timer_us(source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.timer.handle,
                //                           source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.timer.period,
                //                           source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.timer.callback,
                //                           (void *)source);

                /* set timer start timestamp as the first data timestamp */
                source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.data_timestamp = source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.timer.timestamp;
            }

            break;
        #endif /* MTK_GAMING_MODE_HEADSET || AIR_GAMING_MODE_DONGLE_ENABLE */

        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE:
            if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0) ||
                (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1))
            {
                /* get the first timestamp */
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.data_timestamp);
            }

            break;
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

        default:
            break;
    }
}
#endif /* MTK_AUDIO_TRANSMITTER_ENABLE */

#ifdef MTK_AUDIO_BT_COMMON_ENABLE
void dsp_start_stream_in_bt_common(mcu2dsp_start_param_p start_param, SOURCE source)
{
    UNUSED(start_param);

    DSP_MW_LOG_I("Stream in bt common start:%d, %d\r\n", 2, source->param.bt_dl.scenario_type, source->param.bt_dl.scenario_sub_id);

    switch(source->param.bt_dl.scenario_type)
    {
        #if defined(MTK_GAMING_MODE_HEADSET) || defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            if(source->param.bt_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT)
            {
                /* start the first timer */
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.timer.timestamp);
                // hal_gpt_sw_start_timer_us(source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.timer.handle,
                //                         source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.timer.period,
                //                         source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.timer.callback,
                //                         (void *)source);

                /* set timer start timestamp as the first data timestamp */
                source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.data_timestamp = source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.timer.timestamp;
            }

            break;
        #endif /* MTK_GAMING_MODE_HEADSET || AIR_GAMING_MODE_DONGLE_ENABLE */

        default:
            break;
    }

}
#endif /* MTK_AUDIO_BT_COMMON_ENABLE */

void dsp_start_stream_in(mcu2dsp_start_param_p start_param, SOURCE source)
{
    if (start_param != NULL) {
        switch (start_param->param.stream_in) {
#ifdef AIR_AUDIO_HARDWARE_ENABLE
            case STREAM_IN_AFE:
                dsp_start_stream_in_afe(start_param, source);
                break;
#endif /* AIR_AUDIO_HARDWARE_ENABLE */
#ifdef MTK_BT_HFP_ENABLE
            case STREAM_IN_HFP:
                break;
#endif
#ifdef AIR_BT_CODEC_BLE_ENABLED
            case STREAM_IN_BLE:
                break;
#endif
#ifdef MTK_BT_A2DP_ENABLE
            case STREAM_IN_A2DP:
                dsp_start_stream_in_a2dp(start_param, source);
                break;
#endif
            case STREAM_IN_PLAYBACK:
                break;
            case STREAM_IN_VP:
                break;
            #ifdef MTK_AUDIO_TRANSMITTER_ENABLE
            case STREAM_IN_AUDIO_TRANSMITTER:
                dsp_start_stream_in_audio_transmitter(start_param, source);
                break;
            #endif /* MTK_AUDIO_TRANSMITTER_ENABLE */
            #ifdef MTK_AUDIO_BT_COMMON_ENABLE
            case STREAM_IN_BT_COMMON:
                dsp_start_stream_in_bt_common(start_param, source);
                break;
            #endif /* MTK_AUDIO_BT_COMMON_ENABLE */
            default:
                break;
        }
    }
}

#ifdef AIR_AUDIO_HARDWARE_ENABLE
void dsp_start_stream_out_afe(mcu2dsp_start_param_p start_param, SINK sink)
{
    sink->param.audio.AfeBlkControl.u4awsflag = start_param->stream_out_param.afe.mce_flag;
    //sink->param.audio.AfeBlkControl.u4awsflag = TRUE;// synchornize headset project & MCE
    sink->param.audio.aws_sync_request        = start_param->stream_out_param.afe.aws_sync_request;
    sink->param.audio.aws_sync_time           = start_param->stream_out_param.afe.aws_sync_time;
    DSP_MW_LOG_I("Stream out afe start MCE:%d %d %d", 3, sink->param.audio.AfeBlkControl.u4awsflag, sink->param.audio.aws_sync_request, sink->param.audio.aws_sync_time);
}
#endif /* AIR_AUDIO_HARDWARE_ENABLE */

#ifdef MTK_AUDIO_TRANSMITTER_ENABLE
void dsp_start_stream_out_audio_transmitter(mcu2dsp_start_param_p start_param, SINK sink)
{
    UNUSED(start_param);

    DSP_MW_LOG_I("Stream out audio transmitter start:%d, %d\r\n", 2, sink->param.data_ul.scenario_type, sink->param.data_ul.scenario_sub_id);
}
#endif /* MTK_AUDIO_TRANSMITTER_ENABLE */

#ifdef MTK_AUDIO_BT_COMMON_ENABLE
void dsp_start_stream_out_bt_common(mcu2dsp_start_param_p start_param, SINK sink)
{
    UNUSED(start_param);

    DSP_MW_LOG_I("Stream out bt common start:%d, %d\r\n", 2, sink->param.bt_ul.scenario_type, sink->param.bt_ul.scenario_sub_id);
}
#endif /* MTK_AUDIO_BT_COMMON_ENABLE */

void dsp_start_stream_out(mcu2dsp_start_param_p start_param, SINK sink)
{
    if (start_param != NULL) {
        switch (start_param->param.stream_out) {
#ifdef AIR_AUDIO_HARDWARE_ENABLE
            case STREAM_OUT_AFE:
                dsp_start_stream_out_afe(start_param, sink);
                break;
#endif /* AIR_AUDIO_HARDWARE_ENABLE */
#ifdef MTK_BT_HFP_ENABLE
            case STREAM_OUT_HFP:
                break;
#endif
#ifdef AIR_BT_CODEC_BLE_ENABLED
            case STREAM_OUT_BLE:
                break;
#endif
            case STREAM_OUT_RECORD:
                break;
            #ifdef MTK_AUDIO_TRANSMITTER_ENABLE
            case STREAM_OUT_AUDIO_TRANSMITTER:
                dsp_start_stream_out_audio_transmitter(start_param, sink);
                break;
            #endif /* MTK_AUDIO_TRANSMITTER_ENABLE */
            #ifdef MTK_AUDIO_BT_COMMON_ENABLE
            case STREAM_OUT_BT_COMMON:
                dsp_start_stream_out_bt_common(start_param, sink);
                break;
            #endif /* MTK_AUDIO_BT_COMMON_ENABLE */
            default:
                break;
        }
    }
}


void dsp_trigger_suspend (SOURCE source, SINK sink)
{
#ifdef AIR_AUDIO_HARDWARE_ENABLE
    afe_amp_keep_enable_state(TRUE);
    if(source != NULL) {
        audio_ops_trigger(source, AFE_PCM_TRIGGER_SUSPEND);
    }

    if (sink != NULL) {
        audio_ops_trigger(sink, AFE_PCM_TRIGGER_SUSPEND);
    }
#else
    UNUSED(source);
    UNUSED(sink);
#endif /* AIR_AUDIO_HARDWARE_ENABLE */
}

void dsp_trigger_resume (SOURCE source, SINK sink)
{
#ifdef AIR_AUDIO_HARDWARE_ENABLE
    if (source != NULL) {
        if (!audio_ops_trigger(source, AFE_PCM_TRIGGER_RESUME)) {
            source->param.audio.mute_flag = TRUE;
            source->param.audio.pop_noise_pkt_num = 0;
        }
    }

    if (sink != NULL) {
        audio_ops_trigger(sink, AFE_PCM_TRIGGER_RESUME);
    }
    afe_amp_keep_enable_state(FALSE);
#else
    UNUSED(source);
    UNUSED(sink);
#endif /* AIR_AUDIO_HARDWARE_ENABLE */
}
#ifdef MTK_BT_A2DP_ENABLE/* A2DP CCNI callback function */
void CB_N9_A2DP_OPEN(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
#if 0
    //return;
#else
    UNUSED(ack);
    DSP_MW_LOG_I("A2DP OPEN\r\n", 0);

    /* remap to non-cacheable address */

    mcu2dsp_open_param_p open_param;
    open_param = (mcu2dsp_open_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    DSP_REMAP_SHARE_INFO(open_param->stream_in_param.a2dp.p_share_info, AVM_SHARE_BUF_INFO_PTR);
    DSP_MW_LOG_I("A2DP share info %X\r\n", 1, open_param->stream_in_param.a2dp.p_share_info->StartAddr);
    DSP_REMAP_SHARE_INFO(open_param->stream_in_param.a2dp.p_asi_buf, uint32_t *);
    DSP_REMAP_SHARE_INFO(open_param->stream_in_param.a2dp.p_min_gap_buf, uint32_t *);
    DSP_REMAP_SHARE_INFO(open_param->stream_in_param.a2dp.p_pcdc_anchor_info_buf, uint32_t *);
    DSP_REMAP_SHARE_INFO(open_param->stream_in_param.a2dp.p_current_bit_rate, uint32_t *);
    DSP_REMAP_SHARE_INFO(open_param->stream_in_param.a2dp.bt_inf_address, uint32_t);
    DSP_REMAP_SHARE_INFO(open_param->stream_in_param.a2dp.p_afe_buf_report, uint32_t);
#ifdef ENABLE_HWSRC_CLKSKEW
#ifdef AIR_HWSRC_TX_TRACKING_ENABLE
    open_param->stream_out_param.afe.clkskew_mode = CLK_SKEW_V1;
#endif
#endif

#ifdef MTK_GAMING_MODE_HEADSET
    DSP_REMAP_SHARE_INFO(open_param->stream_in_param.a2dp.ull_clk_info_address, uint32_t *);
    MCE_Update_BtClkOffsetAddr(open_param->stream_in_param.a2dp.ull_clk_info_address,ULL_CLK_Offset);
    DSP_MW_LOG_I("[TEST]ull_clk_info_address:0x%x",1,open_param->stream_in_param.a2dp.ull_clk_info_address);
#endif
    n9_a2dp_if.source   = dsp_open_stream_in(open_param);
#if MTK_HWSRC_IN_STREAM
    open_param->stream_out_param.afe.sampling_rate = 48000;
#endif
#ifdef AIR_A2DP_DRC_TO_USE_DGAIN_ENABLE
    open_param->stream_out_param.afe.hw_gain = false;
#endif
    n9_a2dp_if.sink     = dsp_open_stream_out(open_param);
    n9_a2dp_if.transform = NULL;

    DSP_MW_LOG_I("A2DP OPEN Finish\r\n", 0);
    //PIC
#endif
}

void CB_N9_A2DP_START(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
#if 0
    //aud_msg_ack(MSG_MCU2DSP_BT_AUDIO_DL_START | 0x8000, FALSE);
    //return;
#else
    U32 gpt_timer;
    UNUSED(ack);
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_timer);
    DSP_MW_LOG_I("A2DP START Time:%d\r\n", 1, gpt_timer);


    /* remap to non-cacheable address */
    mcu2dsp_start_param_p start_param;
    start_param = (mcu2dsp_start_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);


    dsp_start_stream_in (start_param, n9_a2dp_if.source);
    dsp_start_stream_out(start_param, n9_a2dp_if.sink);
    n9_a2dp_if.sink->param.audio.AfeBlkControl.u4awsflag = 1;
    DSP_MW_LOG_I("A2DP notify cnt address 0x%x Start Ro :%d", 2, &(n9_a2dp_if.source->streamBuffer.AVMBufferInfo.NotifyCount),n9_a2dp_if.source->streamBuffer.AVMBufferInfo.ReadIndex);
    #ifdef MTK_BT_A2DP_VENDOR_BC_ENABLE
    if((n9_a2dp_if.source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR) && (n9_a2dp_if.source->param.n9_a2dp.sink_latency == 0))
    {
        n9_a2dp_if.sink->param.audio.AfeBlkControl.u4awsflag = FALSE;
        DSP_MW_LOG_I("vendor BC force aws flag to be false", 0);
    }
    #endif
    //n9_a2dp_if.sink->param.audio.AfeBlkControl.u4awsflag = FALSE;

    n9_a2dp_if.source->param.n9_a2dp.mce_flag = n9_a2dp_if.sink->param.audio.AfeBlkControl.u4awsflag;


    DSP_MW_LOG_I("A2DP latency :%d", 1,n9_a2dp_if.source->param.n9_a2dp.sink_latency);
    stream_feature_configure_resolution((stream_feature_list_ptr_t)n9_a2dp_if.pfeature_table, RESOLUTION_32BIT, CONFIG_DECODER);
#if MTK_HWSRC_IN_STREAM
    U32 samplerate;//modify for ASRC
    //samplerate = afe_get_audio_device_samplerate(n9_a2dp_if.sink->param.audio.audio_device, n9_a2dp_if.sink->param.audio.audio_interface) /1000 ; //modify for SRC
    samplerate = (n9_a2dp_if.sink->param.audio.rate)/1000;
    DSP_MW_LOG_I("afe_get_audio_device_samplerate = %d\r\n",1,samplerate);//modify for ASRC
    stream_feature_configure_src(n9_a2dp_if.pfeature_table, RESOLUTION_32BIT, RESOLUTION_32BIT, samplerate, 2048);
    if(n9_a2dp_if.source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_SBC)
    DSP_MW_LOG_I("BT_A2DP_CODEC_SBC samplerate %d fixed point %d", 2,samplerate,2048);
    if(n9_a2dp_if.source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_AAC)
    DSP_MW_LOG_I("BT_A2DP_CODEC_AAC samplerate %d fixed point %d", 2,samplerate,2048);
    if(n9_a2dp_if.source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR)
    DSP_MW_LOG_I("BT_A2DP_CODEC_VENDOR samplerate %d fixed point %d", 2,samplerate,2048);
#endif

    n9_a2dp_if.transform = TrasformAudio2Audio(n9_a2dp_if.source, n9_a2dp_if.sink, (stream_feature_list_ptr_t)n9_a2dp_if.pfeature_table);
    n9_a2dp_if.source->param.n9_a2dp.DspReportStartId = msg.ccni_message[0]>>16|0x8000;
    n9_a2dp_if.sink->param.audio.afe_wait_play_en_cnt = 0;
    #ifdef AIR_AUDIO_HARDWARE_ENABLE
    afe_enable_audio_irq(afe_irq_request_number(AUDIO_DIGITAL_BLOCK_MEM_DL1), n9_a2dp_if.sink->param.audio.rate, n9_a2dp_if.sink->param.audio.count);
    #endif /* AIR_AUDIO_HARDWARE_ENABLE */
    if (n9_a2dp_if.transform == NULL)
    {
        DSP_MW_LOG_E("A2DP START transform failed", 0);
    }
    // *(volatile uint32_t *)0x70000F58 = 0x00000208; //ZCD_CON2
#endif
    #ifdef AIR_SILENCE_DETECTION_ENABLE
    Sink_Audio_SilenceDetection_Register(n9_a2dp_if.sink);
    #endif
}
#ifdef MTK_GAMING_MODE_HEADSET
static uint32_t g_play_en_gpt_handler = NULL;
#endif


void CB_N9_A2DP_STOP(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("A2DP STOP\r\n", 0);
    #ifdef AIR_SILENCE_DETECTION_ENABLE
    Sink_Audio_SilenceDetection_Unregister(n9_a2dp_if.sink);
    #endif
    #ifdef TRIGGER_A2DP_PROCSESS_TIMER
    if(trig_a2dp_proc_timer) {
        xTimerStop(trig_a2dp_proc_timer, 0);
    }
    #endif /*TRIGGER_A2DP_PROCSESS_TIMER*/
    #ifdef PLAY_EN_CHECK_TIMER
    if(playen_check_timer) {
        xTimerStop(playen_check_timer, 0);
    }
    #endif
    #ifdef MTK_GAMING_MODE_HEADSET
    if(g_play_en_gpt_handler) {
        hal_gpt_sw_stop_timer_us(g_play_en_gpt_handler);
    }
    #endif
    if (n9_a2dp_if.transform != NULL)
    {
        StreamDSPClose(n9_a2dp_if.transform->source,n9_a2dp_if.transform->sink,msg.ccni_message[0]>>16|0x8000);
    }
    n9_a2dp_if.transform = NULL;
    DSP_MW_LOG_I("A2DP STOP Finish\r\n", 0);
}

void CB_N9_A2DP_CLOSE(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{

    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("A2DP CLOSE\r\n", 0);
    SourceClose(n9_a2dp_if.source);
    SinkClose(n9_a2dp_if.sink);
    DSP_PIC_FeatureDeinit(n9_a2dp_if.pfeature_table);
    DSP_MW_LOG_I("aac feature PIC unload : %d %d", 2, *(n9_a2dp_if.pfeature_table),*(n9_a2dp_if.pfeature_table));
    memset(&n9_a2dp_if,0,sizeof(CONNECTION_IF));

    DSP_MW_LOG_I("A2DP CLOSE Finish\r\n", 0);
}

void CB_N9_A2DP_CLOCKSKEW(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    //clock skew to adjust hw src
}

void CB_N9_A2DP_SUSPEND(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("A2DP SUSPEND\r\n", 0);

    dsp_trigger_suspend(n9_a2dp_if.source, n9_a2dp_if.sink);


}

void CB_N9_A2DP_RESUME(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("A2DP RESUME\r\n", 0);


    dsp_trigger_resume(n9_a2dp_if.source, n9_a2dp_if.sink);

#ifdef AIR_AUDIO_HARDWARE_ENABLE
    afe_amp_keep_enable_state(FALSE);
#endif
}

#endif/* End A2DP CCNI callback function */

#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
extern void Source_device_set_para(hal_audio_device_parameter_t *device_handle);

void dsp_detachable_config(CONNECTION_IF *application_ptr,hal_ccni_message_t msg)
{
    audio_transmitter_scenario_t scenario_id;
    audio_transmitter_scenario_sub_id_t sub_id;
    mcu2dsp_open_param_p open_param;
    SOURCE source;
    audio_channel pre_ch,ch;
    au_afe_open_param_t afe_open;
    scenario_id = (msg.ccni_message[0] >> 8) & 0xFF;
    sub_id.scenario_id  = (msg.ccni_message[0] & 0xFF);
    DSP_MW_LOG_I("dsp_detachable_config", 0);
    if(msg.ccni_message[1]!=NULL){
        open_param = (mcu2dsp_open_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
        open_param->stream_in_param.afe.stream_channel = application_ptr->source->param.audio.stream_channel;
        open_param->stream_in_param.afe.memory = application_ptr->source->param.audio.memory;
        if(application_ptr->source->param.audio.echo_reference) {
            open_param->stream_in_param.afe.memory |= HAL_AUDIO_MEM3;
        }
        // re-use parameters
        open_param->stream_in_param.afe.irq_period = application_ptr->source->param.audio.period;
        open_param->stream_in_param.afe.sampling_rate = application_ptr->source->param.audio.rate;
        open_param->stream_in_param.afe.stream_out_sampling_rate = application_ptr->source->param.audio.src_rate;
        open_param->stream_in_param.afe.format = application_ptr->source->param.audio.format;
        open_param->stream_in_param.afe.frame_number = application_ptr->source->param.audio.Buffer_Frame_Num;
        open_param->stream_in_param.afe.frame_size = application_ptr->source->param.audio.Frame_Size;

        afe_open.audio_device = open_param->stream_in_param.afe.audio_device;
        afe_open.audio_device1 = open_param->stream_in_param.afe.audio_device1;
        afe_open.audio_device2 = open_param->stream_in_param.afe.audio_device2;
        afe_open.audio_device3 = open_param->stream_in_param.afe.audio_device3;
        afe_open.audio_device4 = open_param->stream_in_param.afe.audio_device4;
        afe_open.audio_device5 = open_param->stream_in_param.afe.audio_device5;
        pre_ch = afe_get_audio_channel_by_au_afe_open_param(afe_open);
        afe_open.audio_device = open_param->stream_in_param.afe.audio_device;
        afe_open.audio_device1 = open_param->stream_in_param.afe.audio_device1;
        afe_open.audio_device2 = open_param->stream_in_param.afe.audio_device2;
        afe_open.audio_device3 = open_param->stream_in_param.afe.audio_device3;
        afe_open.audio_device4 = open_param->stream_in_param.afe.audio_device4;
        afe_open.audio_device5 = open_param->stream_in_param.afe.audio_device5;
        ch = afe_get_audio_channel_by_au_afe_open_param(afe_open);
        if(pre_ch!=ch){
            HAL_AUDIO_LOG_ERROR("pre_ch=%d != ch=%d", 2,pre_ch,ch);
            platform_assert("please check open paraters.",__FILE__,__LINE__);
        }
        source = dsp_open_stream_in_afe(open_param);
        hal_audio_path_parameter_t *path_handle = &source->param.audio.path_handle;
        hal_audio_device_parameter_t *device_handle = &source->param.audio.device_handle;
        hal_audio_memory_parameter_t *mem_handle = &source->param.audio.mem_handle;//modify for ab1568
#ifdef ENABLE_2A2D_TEST
        hal_audio_device_parameter_t *device_handle1 = &source->param.audio.device_handle1;
        hal_audio_device_parameter_t *device_handle2 = &source->param.audio.device_handle2;
        hal_audio_device_parameter_t *device_handle3 = &source->param.audio.device_handle3;
        hal_audio_device_parameter_t *device_handle4 = &source->param.audio.device_handle4;
        hal_audio_device_parameter_t *device_handle5 = &source->param.audio.device_handle5;
        hal_audio_device_parameter_t *device_handle6 = &source->param.audio.device_handle6;
        hal_audio_device_parameter_t *device_handle7 = &source->param.audio.device_handle7;
        AUDIO_PARAMETER *pAudPara = &source->param.audio;
        pAudPara->audio_device                   = gAudioCtrl.Afe.AfeULSetting.audio_device;
#ifdef ENABLE_2A2D_TEST
        pAudPara->audio_device1                  = gAudioCtrl.Afe.AfeULSetting.audio_device1;
        pAudPara->audio_device2                  = gAudioCtrl.Afe.AfeULSetting.audio_device2;
        pAudPara->audio_device3                  = gAudioCtrl.Afe.AfeULSetting.audio_device3;
        pAudPara->audio_device4                  = gAudioCtrl.Afe.AfeULSetting.audio_device4;
        pAudPara->audio_device5                  = gAudioCtrl.Afe.AfeULSetting.audio_device5;
        pAudPara->audio_device6                  = gAudioCtrl.Afe.AfeULSetting.audio_device6;
        pAudPara->audio_device7                  = gAudioCtrl.Afe.AfeULSetting.audio_device7;
#endif
        pAudPara->audio_interface                = gAudioCtrl.Afe.AfeULSetting.audio_interface;
#ifdef ENABLE_2A2D_TEST
        pAudPara->audio_interface1               = gAudioCtrl.Afe.AfeULSetting.audio_interface1;
        pAudPara->audio_interface2               = gAudioCtrl.Afe.AfeULSetting.audio_interface2;
        pAudPara->audio_interface3               = gAudioCtrl.Afe.AfeULSetting.audio_interface3;
        pAudPara->audio_interface4               = gAudioCtrl.Afe.AfeULSetting.audio_interface4;
        pAudPara->audio_interface5               = gAudioCtrl.Afe.AfeULSetting.audio_interface5;
        pAudPara->audio_interface6               = gAudioCtrl.Afe.AfeULSetting.audio_interface6;
        pAudPara->audio_interface7               = gAudioCtrl.Afe.AfeULSetting.audio_interface7;
#endif
#endif
        uint32_t i;
        hal_audio_path_port_parameter_t input_port_parameters, output_port_parameters;
        input_port_parameters.device_interface = pAudPara->audio_interface;
        output_port_parameters.memory_select = mem_handle->memory_select&(~HAL_AUDIO_MEMORY_UL_AWB2);
#ifdef ENABLE_2A2D_TEST
        hal_audio_device_t path_audio_device[HAL_AUDIO_PATH_SUPPORT_SEQUENCE] = {pAudPara->audio_device,pAudPara->audio_device1,pAudPara->audio_device2,pAudPara->audio_device3};
        hal_audio_device_interface_t device_interface[HAL_AUDIO_PATH_SUPPORT_SEQUENCE]= {pAudPara->audio_interface,pAudPara->audio_interface1,pAudPara->audio_interface2,pAudPara->audio_interface3};
        hal_audio_memory_selection_t memory_select[HAL_AUDIO_PATH_SUPPORT_SEQUENCE]= {HAL_AUDIO_MEMORY_UL_VUL1, HAL_AUDIO_MEMORY_UL_VUL1 ,HAL_AUDIO_MEMORY_UL_VUL2, HAL_AUDIO_MEMORY_UL_VUL2};
#else
        hal_audio_device_t path_audio_device[HAL_AUDIO_PATH_SUPPORT_SEQUENCE] = {pAudPara->audio_device,pAudPara->audio_device};
        hal_audio_device_interface_t device_interface[HAL_AUDIO_PATH_SUPPORT_SEQUENCE]= {pAudPara->audio_interface,pAudPara->audio_interface};
        hal_audio_memory_selection_t memory_select[HAL_AUDIO_PATH_SUPPORT_SEQUENCE]= {output_port_parameters.memory_select, output_port_parameters.memory_select};
#endif
        for (i=0 ; i<path_handle->connection_number ; i++) {
            input_port_parameters.device_interface = device_interface[i];
            output_port_parameters.memory_select = memory_select[i];
            path_handle->input.interconn_sequence[i]  = stream_audio_convert_control_to_interconn(path_audio_device[i], input_port_parameters, i, true);
            path_handle->output.interconn_sequence[i] = stream_audio_convert_control_to_interconn(HAL_AUDIO_CONTROL_MEMORY_INTERFACE, output_port_parameters, i, true);
        }
        if ((pAudPara->audio_device)&(HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL|HAL_AUDIO_CONTROL_DEVICE_LINE_IN_DUAL|HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL|HAL_AUDIO_CONTROL_DEVICE_ANC|HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER|HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE)) {
            device_handle->common.rate = pAudPara->rate;
            device_handle->common.device_interface = pAudPara->audio_interface;
#ifdef ENABLE_2A2D_TEST
            device_handle1->common.rate = pAudPara->rate;
            device_handle1->common.device_interface = pAudPara->audio_interface1;
            device_handle2->common.rate = pAudPara->rate;
            device_handle2->common.device_interface = pAudPara->audio_interface2;
            device_handle3->common.rate = pAudPara->rate;
            device_handle3->common.device_interface = pAudPara->audio_interface3;
            device_handle4->common.rate = pAudPara->rate;
            device_handle4->common.device_interface = pAudPara->audio_interface4;
            device_handle5->common.rate = pAudPara->rate;
            device_handle5->common.device_interface = pAudPara->audio_interface5;
            device_handle6->common.rate = pAudPara->rate;
            device_handle6->common.device_interface = pAudPara->audio_interface6;
            device_handle7->common.rate = pAudPara->rate;
            device_handle7->common.device_interface = pAudPara->audio_interface7;
#endif
            DSP_MW_LOG_I("set device common.rate %d,source rate %d",2,device_handle->common.rate,pAudPara->rate);
        }

        if (pAudPara->audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
            device_handle->i2s_slave.memory_select = mem_handle->memory_select;
        }

        device_handle->common.audio_device = pAudPara->audio_device;
#ifdef ENABLE_2A2D_TEST
        device_handle1->common.audio_device = pAudPara->audio_device1;
        device_handle2->common.audio_device = pAudPara->audio_device2;
        device_handle3->common.audio_device = pAudPara->audio_device3;
        device_handle4->common.audio_device = pAudPara->audio_device4;
        device_handle5->common.audio_device = pAudPara->audio_device5;
        device_handle6->common.audio_device = pAudPara->audio_device6;
        device_handle7->common.audio_device = pAudPara->audio_device7;
#endif
        Source_device_set_para(device_handle);
#ifdef ENABLE_2A2D_TEST
        Source_device_set_para(device_handle1);
        Source_device_set_para(device_handle2);
        Source_device_set_para(device_handle3);
        Source_device_set_para(device_handle4);
        Source_device_set_para(device_handle5);
        Source_device_set_para(device_handle6);
        Source_device_set_para(device_handle7);
#endif
    }
    DSP_MW_LOG_I("dsp_detachable_config finish", 0);
}
#endif

#ifdef MTK_BT_HFP_ENABLE
bool ScoDlStopFlag;
bool g_esco_dl_open_flag = false;
#ifdef AIR_HWSRC_RX_TRACKING_ENABLE
bool g_esco_ul_open_flag = false;
#endif
#ifdef AIR_HFP_DNN_PATH_ENABLE
CONNECTION_IF sco_ul_dnn_if;
#endif

/* eSCO CCNI callback function */
void CB_N9_SCO_UL_OPEN(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    U32 scenario = (msg.ccni_message[0] & 0x00ff);
    DSP_MW_LOG_I("eSCO UL OPEN, scenario:%d", 1, scenario);

    mcu2dsp_open_param_p open_param;
    open_param = (mcu2dsp_open_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    DSP_REMAP_SHARE_INFO(open_param->stream_out_param.hfp.p_share_info, AVM_SHARE_BUF_INFO_PTR);

    if((scenario == AUDIO_DSP_CODEC_TYPE_CVSD) || (scenario == AUDIO_DSP_CODEC_TYPE_MSBC)) {
        #ifdef AIR_HWSRC_RX_TRACKING_ENABLE
        g_esco_ul_open_flag = true;
        #endif
        n9_sco_ul_if.source   = dsp_open_stream_in(open_param);
        #ifdef AIR_HWSRC_RX_TRACKING_ENABLE
        g_esco_ul_open_flag = false;
        #endif
        n9_sco_ul_if.sink     = dsp_open_stream_out(open_param);
        n9_sco_ul_if.transform = NULL;
        //DSP_MW_LOG_I("eSCO UL OPEN Finish", 0);
    }
#ifdef AIR_HFP_DNN_PATH_ENABLE
    else if (scenario == AUDIO_DSP_CODEC_TYPE_DNN) {
        sco_ul_dnn_if.source   = dsp_open_stream_in(open_param);
        sco_ul_dnn_if.sink     = dsp_open_stream_out(open_param);
        sco_ul_dnn_if.transform = NULL;
        sco_ul_dnn_if.pfeature_table = (stream_feature_list_ptr_t)&stream_feature_list_hfp_uplink_dnn;
        DSP_Callback_PreloaderConfig(sco_ul_dnn_if.pfeature_table);
        //DSP_MW_LOG_I("eSCO UL DNN Open Finish", 0);
        //*((volatile uint32_t *)(0xc00001dc)) = 0x1;
        //*((volatile uint32_t *)(0xc00001f0)) = 0x4A42A42;
    }
#endif
}


void CB_N9_SCO_UL_START(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    U32 scenario = (msg.ccni_message[0] & 0x00ff);
    U32 gpt_timer;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_timer);
    DSP_MW_LOG_I("eSCO UL START, scenario:%d", 1, scenario);

    mcu2dsp_start_param_p start_param;
    start_param = (mcu2dsp_start_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);

    if(scenario == 0) {
        dsp_start_stream_in (start_param, n9_sco_ul_if.source);
        dsp_start_stream_out(start_param, n9_sco_ul_if.sink);
        n9_sco_ul_if.source->param.audio.AfeBlkControl.u4awsflag = 1;
        stream_feature_configure_resolution((stream_feature_list_ptr_t)n9_sco_ul_if.pfeature_table, RESOLUTION_16BIT, 0);
        n9_sco_ul_if.transform = TrasformAudio2Audio(n9_sco_ul_if.source, n9_sco_ul_if.sink, n9_sco_ul_if.pfeature_table);
        if (n9_sco_ul_if.transform == NULL) {
           DSP_MW_LOG_E("SCO UL transform failed", 0);
        }
        //DSP_MW_LOG_I("eSCO UL START finish",0);
    }
#ifdef AIR_HFP_DNN_PATH_ENABLE
    else if (scenario == AUDIO_DSP_CODEC_TYPE_DNN) {
        dsp_start_stream_in (start_param, sco_ul_dnn_if.source);
        dsp_start_stream_out(start_param, sco_ul_dnn_if.sink);
        sco_ul_dnn_if.source->param.audio.AfeBlkControl.u4awsflag = 0;
        stream_feature_configure_resolution((stream_feature_list_ptr_t)sco_ul_dnn_if.pfeature_table, RESOLUTION_16BIT, 0);
        sco_ul_dnn_if.transform = TrasformAudio2Audio(sco_ul_dnn_if.source, sco_ul_dnn_if.sink, sco_ul_dnn_if.pfeature_table);
        if (sco_ul_dnn_if.transform == NULL) {
           DSP_MW_LOG_E("SCO UL DNN transform failed", 0);
        }
        //DSP_MW_LOG_I("eSCO UL DNN START Finish", 0);
    }
#endif
}

void CB_N9_SCO_UL_STOP(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    U32 scenario = (msg.ccni_message[0] & 0x00ff);
    DSP_MW_LOG_I("eSCO UL STOP, scenario:%d", 1, scenario);

    if(scenario == 0) {
        if (n9_sco_ul_if.transform != NULL) {
            StreamDSPClose(n9_sco_ul_if.transform->source,n9_sco_ul_if.transform->sink,msg.ccni_message[0]>>16|0x8000);
        }
        n9_sco_ul_if.transform = NULL;
        DSP_MW_LOG_I("eSCO UL STOP Finish", 0);
    }
#ifdef AIR_HFP_DNN_PATH_ENABLE
    else if (scenario == AUDIO_DSP_CODEC_TYPE_DNN) {
        if (sco_ul_dnn_if.transform != NULL) {
            StreamDSPClose(sco_ul_dnn_if.transform->source,sco_ul_dnn_if.transform->sink,msg.ccni_message[0]>>16|0x8000);
        }
        sco_ul_dnn_if.transform = NULL;
        DSP_MW_LOG_I("eSCO UL DNN STOP Finish", 0);
    }
#endif
}

void CB_N9_SCO_UL_CLOSE(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    U32 scenario = (msg.ccni_message[0] & 0x00ff);
    if(scenario == 0) {
        SourceClose(n9_sco_ul_if.source);
        SinkClose(n9_sco_ul_if.sink);
        DSP_PIC_FeatureDeinit(n9_sco_ul_if.pfeature_table);
        memset(&n9_sco_ul_if,0,sizeof(CONNECTION_IF));
        DSP_MW_LOG_I("eSCO UL CLOSE Finish", 0);
    }
#ifdef AIR_HFP_DNN_PATH_ENABLE
    else if (scenario == AUDIO_DSP_CODEC_TYPE_DNN) {
        SourceClose(sco_ul_dnn_if.source);
        SinkClose(sco_ul_dnn_if.sink);
        DSP_PIC_FeatureDeinit(sco_ul_dnn_if.pfeature_table);
        memset(&sco_ul_dnn_if,0,sizeof(CONNECTION_IF));
        DSP_MW_LOG_I("eSCO UL DNN CLOSE Finish", 0);
    }
#endif
}

void CB_N9_SCO_UL_PLAY(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);

    if (n9_sco_ul_if.transform != NULL)
    {
        DSP_MW_LOG_I("eSCO UL PLAY\r\n", 0);
        #if 0//moidfy for ab1568
        afe_set_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_VUL1, true, true);
        afe_enable_audio_irq(afe_irq_request_number(AUDIO_DIGITAL_BLOCK_MEM_VUL1), n9_sco_ul_if.source->param.audio.rate, n9_sco_ul_if.source->param.audio.count);

        if (n9_sco_ul_if.source->param.audio.echo_reference) {
            afe_set_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_AWB, true, true);
        }
        #else
        hal_audio_trigger_start_parameter_t start_parameter;
        start_parameter.memory_select = n9_sco_ul_if.source->param.audio.mem_handle.memory_select;
        start_parameter.enable = true;
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&start_parameter, HAL_AUDIO_SET_TRIGGER_MEMORY_START);
        #endif
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &(n9_sco_ul_if.sink->param.n9sco.ul_play_gpt));
    }
    else
    {
        DSP_MW_LOG_I("eSCO UL PLAY when tansform not exist\r\n", 0);
    }
}

void CB_N9_SCO_UL_SUSPEND(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("eSCO UL SUSPEND\r\n", 0);
    dsp_trigger_suspend(n9_sco_ul_if.source, n9_sco_ul_if.sink);

}


void CB_N9_SCO_UL_RESUME(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("eSCO UL RESUME\r\n", 0);
#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
    dsp_detachable_config(&n9_sco_ul_if,msg);
#endif
    dsp_trigger_resume(n9_sco_ul_if.source, n9_sco_ul_if.sink);
}

void CB_N9_SCO_DL_OPEN(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("eSCO DL OPEN\r\n", 0);

    mcu2dsp_open_param_p open_param;
    open_param = (mcu2dsp_open_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    DSP_REMAP_SHARE_INFO(open_param->stream_in_param.hfp.p_share_info, AVM_SHARE_BUF_INFO_PTR);
    DSP_MW_LOG_I("eSCO DL share info->StartAddr: 0x%x, forwarder: 0x%x\r\n", 2, open_param->stream_in_param.hfp.p_share_info->StartAddr, open_param->stream_in_param.hfp.p_share_info->ForwarderAddr);
    DSP_REMAP_SHARE_INFO(open_param->stream_in_param.hfp.bt_inf_address, uint32_t);
    DSP_REMAP_SHARE_INFO(open_param->stream_in_param.hfp.clk_info_address, uint32_t);
#ifdef MTK_AIRDUMP_EN
    DSP_REMAP_SHARE_INFO(open_param->stream_in_param.hfp.p_air_dump_buf, uint32_t);
#endif

    n9_sco_dl_if.source   = dsp_open_stream_in(open_param);
    g_esco_dl_open_flag = true;
    n9_sco_dl_if.sink     = dsp_open_stream_out(open_param);
    g_esco_dl_open_flag = false;
    n9_sco_dl_if.transform = NULL;
    //DSP_MW_LOG_I("eSCO DL OPEN Finish\r\n", 0);
}

void CB_N9_SCO_DL_START(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("eSCO DL START\r\n", 0);

    mcu2dsp_start_param_p start_param;
    start_param = (mcu2dsp_start_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);


    dsp_start_stream_in (start_param, n9_sco_dl_if.source);
    dsp_start_stream_out(start_param, n9_sco_dl_if.sink);


    SCO_Rx_Buf_Ctrl(TRUE);
#if 1
    n9_sco_dl_if.sink->param.audio.AfeBlkControl.u4awsflag = TRUE;
#else
    n9_sco_dl_if.sink->param.audio.AfeBlkControl.u4awsflag = FALSE; // Open DL AFE directly
#endif
    n9_sco_dl_if.transform = TrasformAudio2Audio(n9_sco_dl_if.source, n9_sco_dl_if.sink, n9_sco_dl_if.pfeature_table);
    ScoDlStopFlag = FALSE;

    if (n9_sco_dl_if.transform == NULL)
    {
        DSP_MW_LOG_E("SCO DL transform failed", 0);
    }else{
        n9_sco_dl_if.source->param.n9sco.rx_forwarder_en = TRUE;
        //DSP_MW_LOG_E("SCO DL n9_sco_dl_if.sink->param.audio.rx_forwarder_en: %d ", 1, n9_sco_dl_if.source->param.n9sco.rx_forwarder_en);
        SCO_Rx_Intr_HW_Handler();
        SCO_Rx_Intr_Ctrl(TRUE);
    }
}

void CB_N9_SCO_DL_STOP(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("eSCO DL STOP\r\n", 0);
    ScoDlStopFlag = TRUE;
    if (n9_sco_dl_if.transform != NULL) {
        StreamDSPClose(n9_sco_dl_if.transform->source,n9_sco_dl_if.transform->sink,msg.ccni_message[0]>>16|0x8000);
    }
    n9_sco_dl_if.transform = NULL;
    DSP_MW_LOG_I("eSCO DL STOP Finish\r\n", 0);
}

void CB_N9_SCO_DL_CLOSE(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("eSCO DL CLOSE\r\n", 0);
    SourceClose(n9_sco_dl_if.source);
    SinkClose(n9_sco_dl_if.sink);
    DSP_PIC_FeatureDeinit(n9_sco_dl_if.pfeature_table);
    memset(&n9_sco_dl_if,0,sizeof(CONNECTION_IF));
    DSP_MW_LOG_I("eSCO DL CLOSE Finish\r\n", 0);
}

void CB_N9_SCO_CLOCKSKEW(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    //TBD
}

void CB_N9_SCO_DL_SUSPEND(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("eSCO DL SUSPEND\r\n", 0);

    dsp_trigger_suspend(n9_sco_dl_if.source, n9_sco_dl_if.sink);

}


void CB_N9_SCO_DL_RESUME(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("eSCO DL RESUME\r\n", 0);

    dsp_trigger_resume(n9_sco_dl_if.source, n9_sco_dl_if.sink);
}

void CB_N9_SCO_ULIRQ(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    //printf("CB_N9_SCO_ULIRQ\r\n");
    //n9_sco_ul_if.source->transform->Handler(n9_sco_ul_if.source,n9_sco_ul_if.sink);
    //vTaskResume(DAV_TASK_ID);
}

void CB_N9_SCO_DLIRQ(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    if ((Source_blks[SOURCE_TYPE_N9SCO] == NULL)||(n9_sco_dl_if.transform == NULL))
    {
        DSP_MW_LOG_I("Unexpected N9 DL IRQ", 0);
        return;
    }
    if (Source_blks[SOURCE_TYPE_N9SCO]->param.n9sco.share_info_base_addr->ReadIndex == Source_blks[SOURCE_TYPE_N9SCO]->param.n9sco.share_info_base_addr->WriteIndex)
    {
#ifdef PT_bufferfull
        Source_blks[SOURCE_TYPE_N9SCO]->param.n9sco.share_info_base_addr->bBufferIsFull = TRUE;
#endif
        DSP_MW_LOG_I("SCO DL bufferfull, ro:%d, wo:%d", 2, Source_blks[SOURCE_TYPE_N9SCO]->param.n9sco.share_info_base_addr->ReadIndex, Source_blks[SOURCE_TYPE_N9SCO]->param.n9sco.share_info_base_addr->WriteIndex);
    }
    if (n9_sco_dl_if.source->param.n9sco.IsFirstIRQ)
    {
        n9_sco_dl_if.source->param.n9sco.IsFirstIRQ = FALSE;
        U32 gpt_timer;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_timer);
        DSP_MW_LOG_I("CB_N9_SCO_DLIRQ, First Wo:%d, Ro:%d,GPT : %d\r\n", 3, Source_blks[SOURCE_TYPE_N9SCO]->param.n9sco.share_info_base_addr->WriteIndex, Source_blks[SOURCE_TYPE_N9SCO]->param.n9sco.share_info_base_addr->ReadIndex, gpt_timer);
        if ((n9_sco_dl_if.source != NULL)&&(n9_sco_dl_if.source->transform->Handler != NULL))
        {
            n9_sco_dl_if.source->transform->Handler(n9_sco_dl_if.source,n9_sco_dl_if.sink);
            vTaskResume((TaskHandle_t)DAV_TASK_ID);
        }
    }
    if (ScoDlStopFlag == TRUE)
    {
        vTaskResume((TaskHandle_t)DAV_TASK_ID);
    }
}

void CB_N9_SCO_MICIRQ(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    //printf("CB_N9_SCO_MICIRQ\r\n");
    //n9_sco_ul_if.source->transform->Handler(n9_sco_ul_if.source,n9_sco_ul_if.sink);
    //vTaskResume(DAV_TASK_ID);
}
#endif /* End eSCO CCNI callback function */


void CB_AUDIO_DUMP_INIT(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    uint16_t u2SendID;

    UNUSED(ack);

    u2SendID = (msg.ccni_message[0] & 0x00ff);
    DSP_MW_LOG_I("[CB_AUDIO_DUMP_INIT] CCNI ID: %x, Dump Mask: %x, SendID: %x", 3, msg.ccni_message[0], msg.ccni_message[1], u2SendID);
    if (u2SendID == 0) {
        AudioDumpMask[0] = msg.ccni_message[1];
    } else if (u2SendID == 1) {
        AudioDumpMask[1] = msg.ccni_message[1];
    } else if (u2SendID == 2) {
        AudioDumpDevice = msg.ccni_message[1];
    }

#ifdef MTK_AUDIO_DUMP_BY_SPDIF_ENABLE
    audio_dump_init();
#endif
}

#ifdef AIR_BT_CODEC_BLE_ENABLED
static uint32_t g_dl_sw_timer_handle = 0;
static uint32_t g_ul_sw_timer_handle = 0;

/* BLE CCNI callback function */
void CB_N9_BLE_UL_OPEN(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("[BLE] UL OPEN\r\n", 0);


    mcu2dsp_open_param_p open_param;
    open_param = (mcu2dsp_open_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    DSP_REMAP_SHARE_INFO(open_param->stream_out_param.ble.p_share_info, SHARE_BUFFER_INFO_PTR);

    n9_ble_ul_if.source   = dsp_open_stream_in(open_param);
    n9_ble_ul_if.sink     = dsp_open_stream_out(open_param);
    n9_ble_ul_if.transform = NULL;

    DSP_MW_LOG_I("[BLE] UL OPEN Finish\r\n", 0);
}

void CB_N9_BLE_UL_START(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    U32 gpt_timer;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_timer);
    DSP_MW_LOG_I("[BLE] UL START gpt %d\r\n", 1,gpt_timer);

    mcu2dsp_start_param_p start_param;
    start_param = (mcu2dsp_start_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);

    dsp_start_stream_in (start_param, n9_ble_ul_if.source);
    dsp_start_stream_out(start_param, n9_ble_ul_if.sink);

    n9_ble_ul_if.source->param.audio.AfeBlkControl.u4awsflag = 1;

    n9_ble_ul_if.transform = TrasformAudio2Audio(n9_ble_ul_if.source, n9_ble_ul_if.sink, n9_ble_ul_if.pfeature_table);
    if (n9_ble_ul_if.transform == NULL)
    {
        DSP_MW_LOG_E("[BLE] UL transform failed", 0);
    }
    DSP_MW_LOG_I("[BLE] UL START Finish\r\n",0);
}

void CB_N9_BLE_UL_STOP(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("[BLE] UL STOP\r\n", 0);
    if (n9_ble_ul_if.transform != NULL)
    {
        StreamDSPClose(n9_ble_ul_if.transform->source,n9_ble_ul_if.transform->sink,msg.ccni_message[0]>>16|0x8000);
    }
    if (g_ul_sw_timer_handle != 0)
    {
        hal_gpt_sw_stop_timer_us(g_ul_sw_timer_handle);
        hal_gpt_sw_free_timer(g_ul_sw_timer_handle);
        g_ul_sw_timer_handle = 0;
    }
    n9_ble_ul_if.transform = NULL;
    DSP_MW_LOG_I("[BLE] UL STOP Finish\r\n", 0);
}

EXTERN U8 g_lc3_user_cnt;
EXTERN void LC3_TabDeinit(void);
EXTERN VOID LC3_MemDeinit (void);
void CB_N9_BLE_UL_CLOSE(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);

    SourceClose(n9_ble_ul_if.source);
    SinkClose(n9_ble_ul_if.sink);
    DSP_PIC_FeatureDeinit(n9_ble_ul_if.pfeature_table);

    g_lc3_user_cnt -= 1;
    DSP_MW_LOG_I("g_lc3_user_cnt %d",1,g_lc3_user_cnt);
    if(!g_lc3_user_cnt){
        LC3_TabDeinit();
        LC3_MemDeinit();
        lc3i_plc_std_library_unload();
        if(lc3_ul_pic_type = LC3I_UL_CALL_10_MS){
            lc3i_fft10ms_library_unload();
        }else{
            lc3i_fft7_5ms_library_unload();
        }
    }
    memset(&n9_ble_ul_if,0,sizeof(CONNECTION_IF));
    DSP_MW_LOG_I("[BLE] UL CLOSE Finish\r\n", 0);
}

void CB_N9_BLE_UL_PLAY(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);

    if (n9_ble_ul_if.transform != NULL)
    {
        DSP_MW_LOG_I("[BLE] UL PLAY\r\n", 0);
        afe_set_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_VUL1, true, true);
        afe_enable_audio_irq(afe_irq_request_number(AUDIO_DIGITAL_BLOCK_MEM_VUL1), n9_ble_ul_if.source->param.audio.rate, n9_ble_ul_if.source->param.audio.count);

        if (n9_ble_ul_if.source->param.audio.echo_reference) {
            afe_set_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_AWB, true, true);
        }
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &(n9_ble_ul_if.sink->param.n9ble.ul_play_gpt));
    }
    else
    {
        DSP_MW_LOG_I("[BLE] UL PLAY when tansform not exist\r\n", 0);
    }
}

void CB_N9_BLE_UL_SUSPEND(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("[BLE] UL SUSPEND\r\n", 0);
    dsp_trigger_suspend(n9_ble_ul_if.source, n9_ble_ul_if.sink);

}

void CB_N9_BLE_UL_RESUME(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("[BLE] UL RESUME\r\n", 0);
    dsp_trigger_resume(n9_ble_ul_if.source, n9_ble_ul_if.sink);
}

bool g_n9_ble_dl_open_flag = false;
bool g_n9_ble_dl_gaming_flag = false;

void CB_N9_BLE_DL_OPEN(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("[BLE] DL OPEN\r\n", 0);

    mcu2dsp_open_param_p open_param;
    open_param = (mcu2dsp_open_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    DSP_MW_LOG_I("[BLE] share info add: 0x%x\r\n", 1, open_param->stream_in_param.ble.p_share_info);
    DSP_REMAP_SHARE_INFO(open_param->stream_in_param.ble.p_share_info, SHARE_BUFFER_INFO_PTR);
    DSP_MW_LOG_I("[BLE] share buffer startadd: 0x%x\r\n type: 0x%x", 2, open_param->stream_in_param.ble.p_share_info->startaddr,open_param->stream_in_param.ble.context_type);

    n9_ble_dl_if.source   = dsp_open_stream_in(open_param);
    g_n9_ble_dl_open_flag = true;
    g_n9_ble_dl_gaming_flag = (open_param->stream_in_param.ble.context_type == BLE_CONTEXT_GAME) ? true : false;
    n9_ble_dl_if.sink     = dsp_open_stream_out(open_param);
    g_n9_ble_dl_open_flag = false;
    n9_ble_dl_if.transform = NULL;

    DSP_MW_LOG_I("[BLE] DL OPEN Finish\r\n", 0);
}

void CB_N9_BLE_DL_START(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("[BLE] DL START\r\n", 0);

    mcu2dsp_start_param_p start_param;
    start_param = (mcu2dsp_start_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);


    dsp_start_stream_in (start_param, n9_ble_dl_if.source);
    dsp_start_stream_out(start_param, n9_ble_dl_if.sink);
    n9_ble_dl_if.sink->param.audio.AfeBlkControl.u4awsflag = 1;
    n9_ble_dl_if.transform = TrasformAudio2Audio(n9_ble_dl_if.source, n9_ble_dl_if.sink, n9_ble_dl_if.pfeature_table);
    BleDlStopFlag = FALSE;
    if (n9_ble_dl_if.transform == NULL)
    {
        DSP_MW_LOG_E("[BLE] DL transform failed", 0);
    }
    //*(volatile uint32_t *)0x70000F58 = 0x00000208; //ZCD_CON2
    #ifdef AIR_SILENCE_DETECTION_ENABLE
    Sink_Audio_SilenceDetection_Register(n9_a2dp_if.sink);
    #endif
    DSP_MW_LOG_I("[BLE] DL START Finished\r\n", 0);
}

void CB_N9_BLE_DL_STOP(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("[BLE] DL STOP\r\n", 0);
    #ifdef AIR_SILENCE_DETECTION_ENABLE
    Sink_Audio_SilenceDetection_Unregister(n9_a2dp_if.sink);
    #endif
    BleDlStopFlag = TRUE;
    if (n9_ble_dl_if.transform != NULL)
    {
        StreamDSPClose(n9_ble_dl_if.transform->source,n9_ble_dl_if.transform->sink,msg.ccni_message[0]>>16|0x8000);
    }
    #ifdef PLAY_EN_CHECK_TIMER
    if(playen_check_timer) {
        xTimerStop(playen_check_timer, 0);
    }
    #endif
    if (g_dl_sw_timer_handle != 0)
    {
        hal_audio_afe_disable_play_en();// Disable unarrived play_en
        hal_gpt_sw_stop_timer_us(g_dl_sw_timer_handle);
        hal_gpt_sw_free_timer(g_dl_sw_timer_handle);
        g_dl_sw_timer_handle = 0;
    }
    n9_ble_dl_if.transform = NULL;
    DSP_MW_LOG_I("[BLE] DL STOP Finish\r\n", 0);
}

void CB_N9_BLE_DL_CLOSE(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("[BLE] DL CLOSE\r\n", 0);
    SourceClose(n9_ble_dl_if.source);
    SinkClose(n9_ble_dl_if.sink);
    DSP_PIC_FeatureDeinit(n9_ble_dl_if.pfeature_table);

    g_lc3_user_cnt -= 1;
    DSP_MW_LOG_I("g_lc3_user_cnt %d",1,g_lc3_user_cnt);
    if(!g_lc3_user_cnt){
        LC3_TabDeinit();
        LC3_MemDeinit();
        switch(lc3_dl_pic_type){
            case LC3I_DL_AUDIO_10_MS:
                lc3i_plc_adv_library_unload();
                lc3i_fft10ms_library_unload();
                break;
            case LC3I_DL_AUDIO_7P5_MS:
                lc3i_plc_adv_library_unload();
                lc3i_fft7_5ms_library_unload();
                break;
            case LC3I_DL_CALL_10_MS:
                lc3i_plc_std_library_unload();
                lc3i_fft10ms_library_unload();
                break;
            case LC3I_DL_CALL_7P5_MS:
                lc3i_plc_std_library_unload();
                lc3i_fft7_5ms_library_unload();
                break;
            default:
               break;
        }
    }
    memset(&n9_ble_dl_if,0,sizeof(CONNECTION_IF));
    DSP_MW_LOG_I("[BLE] DL CLOSE Finish\r\n", 0);
}

void CB_N9_BLE_CLOCKSKEW(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    //TBD
}

void CB_N9_BLE_DL_SUSPEND(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("[BLE] DL SUSPEND\r\n", 0);

    dsp_trigger_suspend(n9_ble_dl_if.source, n9_ble_dl_if.sink);

}

void CB_N9_BLE_DL_RESUME(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("[BLE] DL RESUME\r\n", 0);

    dsp_trigger_resume(n9_ble_dl_if.source, n9_ble_dl_if.sink);
}

void CB_N9_BLE_ULIRQ(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    //printf("CB_N9_SCO_ULIRQ\r\n");
    //n9_sco_ul_if.source->transform->Handler(n9_sco_ul_if.source,n9_sco_ul_if.sink);
    //vTaskResume(DAV_TASK_ID);
}

extern bool N9_BLE_SOURCE_ROUTINE(void);
void CB_N9_BLE_DLIRQ(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    /*hal_gpio_set_output(HAL_GPIO_0, 1);
    hal_gpio_set_output(HAL_GPIO_0, 0);*/
    if(N9_BLE_SOURCE_ROUTINE())
    {
        return;
    }

    UNUSED(msg);
    UNUSED(ack);
    if ((Source_blks[SOURCE_TYPE_N9BLE] == NULL)||(n9_ble_dl_if.transform == NULL))
    {
        DSP_MW_LOG_I("[BLE] Unexpected N9 DL IRQ", 0);
        return;
    }
    if (Source_blks[SOURCE_TYPE_N9BLE]->param.n9ble.share_info_base_addr->ReadOffset == Source_blks[SOURCE_TYPE_N9BLE]->param.n9ble.share_info_base_addr->WriteOffset)
    {
        Source_blks[SOURCE_TYPE_N9BLE]->param.n9ble.share_info_base_addr->bBufferIsFull = TRUE;
        DSP_MW_LOG_I("[BLE] DL bufferfull, Wo:%d, Ro:%d", 2, Source_blks[SOURCE_TYPE_N9BLE]->param.n9ble.share_info_base_addr->WriteOffset, Source_blks[SOURCE_TYPE_N9BLE]->param.n9ble.share_info_base_addr->ReadOffset);
    }
    //DSP_MW_LOG_I("[BLE] CB_N9_BLE_DLIRQ, Wo:%d, Ro:%d\r\n", 3, Source_blks[SOURCE_TYPE_N9BLE]->param.n9ble.share_info_base_addr->WriteOffset, Source_blks[SOURCE_TYPE_N9BLE]->param.n9ble.share_info_base_addr->ReadOffset);
    if (n9_ble_dl_if.source->param.n9ble.IsFirstIRQ)
    {
        /*hal_gpio_set_output(HAL_GPIO_1, 1);
        hal_gpio_set_output(HAL_GPIO_1, 0);*/

        n9_ble_dl_if.source->param.n9ble.IsFirstIRQ = FALSE;
        memset((U8*)Source_blks[SOURCE_TYPE_N9BLE]->streamBuffer.ShareBufferInfo.startaddr, 0, N9BLE_setting->N9Ble_source.Frame_Size);

        U32 gpt_timer;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_timer);
        DSP_MW_LOG_I("[BLE] CB_N9_BLE_DLIRQ, First Wo:%d, Ro:%d,GPT : %d\r\n", 3, Source_blks[SOURCE_TYPE_N9BLE]->param.n9ble.share_info_base_addr->WriteOffset, Source_blks[SOURCE_TYPE_N9BLE]->param.n9ble.share_info_base_addr->ReadOffset, gpt_timer);
        if ((n9_ble_dl_if.source != NULL)&&(n9_ble_dl_if.source->transform->Handler != NULL))
        {
            n9_ble_dl_if.source->transform->Handler(n9_ble_dl_if.source,n9_ble_dl_if.sink);
            vTaskResume((TaskHandle_t)DAV_TASK_ID);
        }
    }
    if (BleDlStopFlag == TRUE)
    {
        vTaskResume((TaskHandle_t)DAV_TASK_ID);
    }
}

void CB_N9_BLE_MICIRQ(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    //printf("[BLE] CB_N9_SCO_MICIRQ\r\n");
    //n9_sco_ul_if.source->transform->Handler(n9_sco_ul_if.source,n9_sco_ul_if.sink);
    //vTaskResume(DAV_TASK_ID);
}

/* Below CB_N9_BLE_INIT_PLAY_INFO is used to control how to start the DL/UL flow  */

typedef struct {
    uint32_t iso_interval; /* Unit with BT clock (312.5us) */
    uint32_t dl_timestamp_clk; /* Unit with BT clock (312.5us), indicate the first anchor of DL */
    uint32_t dl_retransmission_window_clk; /* Unit with BT clock (312.5us), valid bit[27:2] */
    uint16_t dl_timestamp_phase; /* Unit with 0.5us, valid value: 0~2499 */
    uint16_t dl_retransmission_window_phase; /* Unit with 0.5us, valid value: 0~2499 */
    uint8_t  dl_ft;
    uint8_t  dl_packet_counter; /* ISO DL packet counter & 0xFF */
    uint8_t  ul_ft;
    uint8_t  ul_packet_counter; /* ISO UL packet counter & 0xFF */
    uint32_t ul_timestamp; /* Unit with BT clock (312.5us), indicate the first anchor of UL */
    uint32_t ISOAnchorClk; /* Unit with BT clock (312.5us), indicate the first anchor of UL */
    uint16_t ISOAnchorPhase; /* Unit with BT clock (312.5us), indicate the first anchor of UL */
    uint8_t  DL_AVMIndex;
} ble_init_play_info_t;


#define BLE_UL_TIME_STREAM_PROCESS  9000
#define BLE_TIME_BT_CLOCK           625 /* unit with 312.5us */
#define BLE_UL_TIME_PLAY_EN_MARGIN  5000
#define BLE_DL_TIME_PLAY_EN_MARGIN  15000

#define BIT_MASK(n)         (1UL << (n))
#define BT_CLOCK_TICK_MASK  (0x0FFFFFFC)
#define BT_CLOCK_MAX_WRAP   (0x10000000)

void LC_Get_CurrBtClk(BTCLK *pCurrCLK, BTPHASE *pCurrPhase)
{
    *pCurrCLK = rBb->rAudioCtl.rRxClk;
    *pCurrPhase = rBb->rAudioCtl.rRxPhs;
    if (*((volatile uint32_t *)(CONN_BT_TIMCON_BASE + 0x011C)) != 0)
    {
        DSP_MW_LOG_E("[LC] DSP get current bt clk when sleep, 0x%x", 1,*((volatile uint32_t *)(CONN_BT_TIMCON_BASE + 0x011C)) );
    }
}

/******************************************************************************
 *  LC_IsBTTimeExpired
 *  Descprition :
 *  return TRUE  : CurrentBTTime >= ExpiredBTTime
 *  return FALSE : CurrentBTTime <  ExpiredBTTime
 *  This API had considered wrap around case, because it check MSB of Valid Clock bit.
 *******************************************************************************/
bool LC_BtClock_IsBTTimeExpired(BTTIME_STRU_PTR pExpiredBTTime, BTTIME_STRU_PTR pCurrentBTTime)
{
    BTCLK ExpiredCLK = pExpiredBTTime->period & 0xFFFFFFC;
    BTCLK CurrentCLK = pCurrentBTTime->period & 0xFFFFFFC;
    BTCLK DeltaTime;

    DeltaTime = CurrentCLK - ExpiredCLK;
    if ((DeltaTime & BIT_MASK(27)) > 0)
    {
        //CurrentCLK <  ExpiredTime
        return FALSE;
    }
    else
    {
        //CurrentCLK >= ExpiredTime
        if ( CurrentCLK == ExpiredCLK)//Same CLK, We have to compare Phase
        {
            return (pCurrentBTTime->phase >= pExpiredBTTime->phase);
        }
        else
        {
            return TRUE;
        }
    }
}
/**
 * LC_Add_us_FromA
 *
 * Add us from a => b = a + n
 *
 * @Author : Wen <wen.huang@airoha.com.tw>
 *
 * @n : In offset unit us
 * @pa : In
 * @pb : out
 * @Return : b = a + n
 */
VOID LC_Add_us_FromA(U32 n, BTTIME_STRU_PTR pa, BTTIME_STRU_PTR pb)
{
    //get cur clk and phase
    BTCLK a_t0 = pa->period & 0xFFFFFFC;
    U32 m;
    m = pa->phase + (n<<1);
    pb->period = (a_t0 + m/625) &BT_CLOCK_TICK_MASK;
    pb->phase = m % 2500;
}

/**
 * LC_Subtract_us_Fromb
 *
 * Subtract us from b => a = b - n
 *
 * @Author : Wen <wen.huang@airoha.com.tw>
 *
 * @n : In offset unit us
 * @pa : out
 * @pb : in
 * @Return : b - n = a
 */
VOID LC_Subtract_us_Fromb(U32 n, BTTIME_STRU_PTR pa, BTTIME_STRU_PTR pb)
{
    BTCLK b_t0 = pb->period & 0xFFFFFFC;
    BTCLK a_t0;
    U32 x;

    n = n<<1;//change to unit 0.5us
    if (pb->phase >= n)
    {
        pa->phase = pb->phase - n;
        pa->period = (b_t0 + pa->phase/625) &BT_CLOCK_TICK_MASK;
    }
    else
    {
        x = (n - pb->phase)%2500;
        pa->phase = (2500 - x)%2500;
        a_t0 = (BT_CLOCK_MAX_WRAP + b_t0 - ((((n - pb->phase)/2500)+(pa->phase > 0))*4)) & BT_CLOCK_TICK_MASK;
        pa->period = a_t0 + pa->phase/625;
    }
}

/**
 * LC_Get_Offset_FromAB
 *
 * get offset us between a and b
 *
 * @Author : Wen <wen.huang@airoha.com.tw>
 *
 * @pa : In Point a
 * @pb : In Point b
 * @Return : b-a = ? us , maximum value 134217726us
 */
U32 LC_Get_Offset_FromAB(BTTIME_STRU_PTR pa, BTTIME_STRU_PTR pb)
{
    BTCLK a_t0 = pa->period & 0xFFFFFFC;
    BTCLK b_t0 = pb->period & 0xFFFFFFC;
    BTCLK CLKOffset;
    U32 Phase;
    if (pa->period <= pb->period)
    {
        CLKOffset = (b_t0-a_t0);
    }
    else
    {
        CLKOffset = (0xFFFFFFF - a_t0 + b_t0 + 1);
    }
    Phase = (CLKOffset*625) - pa->phase + pb->phase;
    return (Phase>>1);
}



static void ble_init_ul_timer_callback(void *user_data)
{
    hal_audio_trigger_start_parameter_t start_parameter;

    UNUSED(user_data);

    /* Enable UL AFE path */
    start_parameter.memory_select = n9_ble_ul_if.source->param.audio.mem_handle.memory_select;
    start_parameter.enable = true;
    hal_audio_set_value((hal_audio_set_value_parameter_t *)&start_parameter, HAL_AUDIO_SET_TRIGGER_MEMORY_START);
    DSP_MW_LOG_I("[BLE] ble first sw ul callback, for initial setting", 0);
    if (g_ul_sw_timer_handle != 0)
    {
        hal_gpt_sw_free_timer(g_ul_sw_timer_handle);
        g_ul_sw_timer_handle = 0;
    }
    if ((n9_ble_ul_if.source != NULL) && (n9_ble_ul_if.source->transform != NULL)) {
        AudioCheckTransformHandle(n9_ble_ul_if.source->transform);
    }
#ifdef MTK_BLE_LATENCY_GPIO_DEBUG
    hal_gpio_toggle_pin(HAL_GPIO_11);
#endif
}

static void ble_init_dl_timer_callback(void *user_data)
{
    UNUSED(user_data);

    if ((n9_ble_dl_if.source != NULL) && (n9_ble_dl_if.source->transform != NULL)) {
        AudioCheckTransformHandle(n9_ble_dl_if.source->transform);
    }
    DSP_MW_LOG_I("[BLE] ble first sw dl callback", 0);

    if (g_dl_sw_timer_handle != 0)
    {
        hal_gpt_sw_free_timer(g_dl_sw_timer_handle);
        g_dl_sw_timer_handle = 0;
    }

#ifdef MTK_BLE_LATENCY_GPIO_DEBUG
    hal_gpio_toggle_pin(HAL_GPIO_8);
#endif
}

static void ble_trigger_ul_stream(ble_init_play_info_t *play_info)
{
    uint16_t curr_intra_clk;
    uint32_t curr_bt_clk, during_time, offset_time, relocate_avm_wptr;
    BTTIME_STRU play_en_time, curr_time, expect_time, first_anchor_time;

    /*
      * decide the time of play en
      *     - margin from frame process done to controller begin to copy
      *     - time of process one frame (AFE buffer -> AVM buffer)
      *     - time of AFE buffer begin to capture one frame data
      *     - time of MIC mute(avoid pop noise) because of AFE HW request
      */
    offset_time = n9_ble_ul_if.source->param.audio.period * 1000 + BLE_UL_TIME_STREAM_PROCESS ;
    first_anchor_time.period = play_info->dl_timestamp_clk;
    first_anchor_time.phase = play_info->dl_timestamp_phase;
    LC_Subtract_us_Fromb(offset_time, &play_en_time, &first_anchor_time);
    n9_ble_ul_if.sink->param.n9ble.predict_timestamp = play_info->ul_timestamp- ((n9_ble_ul_if.sink->param.n9ble.frame_interval<<1)/BLE_TIME_BT_CLOCK);

    LC_Get_CurrBtClk(&curr_bt_clk, &curr_intra_clk);
    curr_time.period = curr_bt_clk;
    curr_time.phase = curr_intra_clk;
    LC_Add_us_FromA(BLE_UL_TIME_PLAY_EN_MARGIN, &curr_time, &expect_time);

    relocate_avm_wptr = play_info->ul_packet_counter;
    while (LC_BtClock_IsBTTimeExpired(&play_en_time, &expect_time) == true) {
        LC_Add_us_FromA(2*n9_ble_ul_if.sink->param.n9ble.frame_interval, &play_en_time, &play_en_time);
        relocate_avm_wptr+=2;
        n9_ble_ul_if.sink->param.n9ble.predict_timestamp += 2*((n9_ble_ul_if.sink->param.n9ble.frame_interval<<1)/BLE_TIME_BT_CLOCK);
    }

    /* re-locate the RPTR of first frame */
    relocate_avm_wptr %= N9BLE_setting->N9Ble_sink.Buffer_Frame_Num;
    relocate_avm_wptr *= N9BLE_setting->N9Ble_sink.Frame_Size;
    N9Ble_update_writeoffset_share_information(n9_ble_ul_if.sink, relocate_avm_wptr);
    DSP_MW_LOG_I("[BLE][sink] initial writeoffset %d ts %d", 2,relocate_avm_wptr, n9_ble_ul_if.sink->param.n9ble.predict_timestamp);

    DSP_MW_LOG_I("[BLE] UL first_anchor_time %d, %d, curr_time %d, %d, expect_time %d, %d, play_en_time %d, %d", 8,
                    first_anchor_time.period, first_anchor_time.phase,
                    curr_time.period, curr_time.phase,
                    expect_time.period, expect_time.phase,
                    play_en_time.period, play_en_time.phase);

    during_time = LC_Get_Offset_FromAB(&curr_time, &play_en_time);
    DSP_MW_LOG_I("[BLE] ble_trigger_ul_stream(), during_time = %d", 1, during_time);
    hal_gpt_sw_get_timer(&g_ul_sw_timer_handle);
    hal_gpt_sw_start_timer_us(g_ul_sw_timer_handle, during_time, ble_init_ul_timer_callback, NULL);
}

static void ble_trigger_dl_stream(ble_init_play_info_t *play_info)
{
    uint16_t native_intra_clk;
    uint32_t during_time, native_bt_clk, retransmission_window;
    BTTIME_STRU play_en_time, curr_time, expect_time;

    /*
      * decide the time of play en
      *     - retransmission window
      */
    retransmission_window = ((play_info->dl_retransmission_window_clk + 4)>>2) * 1250 + play_info->dl_retransmission_window_phase / 2;
    play_en_time.period = play_info->dl_timestamp_clk;
    play_en_time.phase = play_info->dl_timestamp_phase;
    LC_Add_us_FromA(retransmission_window, &play_en_time, &play_en_time);

    LC_Get_CurrBtClk(&curr_time.period, &curr_time.phase);
    LC_Add_us_FromA(BLE_DL_TIME_PLAY_EN_MARGIN, &curr_time, &expect_time);
    if (n9_ble_dl_if.source->param.n9ble.dual_cis_status == DUAL_CIS_WAITING_SUB)
    {
        //n9_ble_dl_if.source->param.n9ble.dual_cis_buffer_offset = (((play_info->dl_timestamp_clk + play_info->dl_ft*play_info->iso_interval) - n9_ble_dl_if.source->param.n9ble.predict_timestamp)*2/n9_ble_dl_if.source->param.n9ble.frame_interval/625)%6;
        n9_ble_dl_if.source->param.n9ble.dual_cis_buffer_offset = 0;
        //Default offset, could be wrong but will be correct later
        DSP_MW_LOG_I("[BLE] Sub CIS start ts : %d", 1, play_en_time.period,play_en_time.period );
        n9_ble_dl_if.source->param.n9ble.dual_cis_status = DUAL_CIS_BOTH_ENABLED;
        return;
    }
    n9_ble_dl_if.source->streamBuffer.ShareBufferInfo.ReadOffset = 0;
    while (LC_BtClock_IsBTTimeExpired(&play_en_time, &expect_time) == true) {
        LC_Add_us_FromA((play_info->iso_interval * BLE_TIME_BT_CLOCK)>>1, &play_en_time, &play_en_time);
        n9_ble_dl_if.source->param.n9ble.predict_timestamp += play_info->iso_interval;
        N9Ble_SourceUpdateLocalReadOffset(n9_ble_dl_if.source,n9_ble_dl_if.source->param.n9ble.frame_per_iso);
    }
    N9Ble_update_readoffset_share_information(n9_ble_dl_if.source, n9_ble_dl_if.source->streamBuffer.ShareBufferInfo.ReadOffset);
    DSP_MW_LOG_I("[BLE] DL start anchor timestamp : %d readoffset : %d", 2,n9_ble_dl_if.source->param.n9ble.predict_timestamp,n9_ble_dl_if.source->streamBuffer.ShareBufferInfo.ReadOffset);

    /* turn on play_en */
    MCE_TransBT2NativeClk(play_en_time.period, play_en_time.phase, &native_bt_clk, &native_intra_clk,BT_CLK_Offset);
    AFE_SET_REG(AFE_MEMIF_PBUF_SIZE, 0, 3<<0);//reduce pbuffer size
    AFE_SET_REG(AFE_MEMIF_MINLEN, 1, 0XF);//reduce pbuffer size

    hal_audio_afe_set_play_en(native_bt_clk,native_intra_clk);
    DSP_MW_LOG_I("[BLE] ble_trigger_dl_stream() native_bt_clk %d, native_intra_clk %d", 2, native_bt_clk, native_intra_clk);

    DSP_MW_LOG_I("[BLE] DL first_anchor_time %d, %d, curr_time %d, %d, expect_time %d, %d, play_en_time %d, %d", 8,
                    play_info->dl_timestamp_clk, play_info->dl_timestamp_phase,
                    curr_time.period, curr_time.phase,
                    expect_time.period, expect_time.phase,
                    play_en_time.period, play_en_time.phase);

    /* start timer for wakeup later stream process */
    during_time = LC_Get_Offset_FromAB(&curr_time, &play_en_time);
    if(n9_ble_dl_if.source->param.n9ble.frame_interval < 10000){
        DSP_MW_LOG_I("[BLE] ble_trigger_dl_stream(), early wakeup %d", 1, (10000 - n9_ble_dl_if.source->param.n9ble.frame_interval));
        during_time -= (10000 - n9_ble_dl_if.source->param.n9ble.frame_interval);
    }
    DSP_MW_LOG_I("[BLE] ble_trigger_dl_stream(), during_time %d", 1, during_time);
    if (n9_ble_dl_if.source->param.n9ble.dual_cis_status == DUAL_CIS_WAITING_MAIN)
    {
        n9_ble_dl_if.source->param.n9ble.dual_cis_status = DUAL_CIS_WAITING_SUB;
    }
    #ifdef PLAY_EN_CHECK_TIMER
    if (playen_check_timer == NULL)
    {
        playen_check_timer = xTimerCreate("PLANEN_CHECK_TIMER", pdMS_TO_TICKS(10), pdTRUE, 0, playen_check_timer_callback);
    }
    if(!playen_check_timer) {
        DSP_MW_LOG_I("playen_check_timer create timer FAIL", 0);
    } else {
        DSP_MW_LOG_I("playen_check_timer create timer PASS", 0);
        xTimerChangePeriod(playen_check_timer, pdMS_TO_TICKS(10), 0);
    }
    #endif
    if (during_time >= 10000)
    {
        during_time -= 5000;
    }

    hal_gpt_sw_get_timer(&g_dl_sw_timer_handle);
    hal_gpt_sw_start_timer_us(g_dl_sw_timer_handle, during_time, ble_init_dl_timer_callback, NULL);
}

void CB_N9_BLE_INIT_PLAY_INFO(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    ble_init_play_info_t *play_info;

    UNUSED(ack);

    DSP_MW_LOG_I("[BLE] CB_N9_BLE_INIT_PLAY_INFO called cis status : %d", 1,n9_ble_dl_if.source->param.n9ble.dual_cis_status);


    /* As the play info don't go through the host check flow, so the ble flow may has closed when run here */


    play_info = (ble_init_play_info_t *)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);

    DSP_MW_LOG_I("[BLE] play_info->iso_interval %d", 1, play_info->iso_interval);
    DSP_MW_LOG_I("[BLE] play_info->dl_timestamp_clk %d", 1, play_info->dl_timestamp_clk);
    DSP_MW_LOG_I("[BLE] play_info->dl_timestamp_phase %d", 1, play_info->dl_timestamp_phase);
    DSP_MW_LOG_I("[BLE] play_info->dl_retransmission_window_clk %d", 1, play_info->dl_retransmission_window_clk);
    DSP_MW_LOG_I("[BLE] play_info->dl_retransmission_window_phase %d", 1, play_info->dl_retransmission_window_phase);
    DSP_MW_LOG_I("[BLE] play_info->dl_ft %d", 1, play_info->dl_ft);
    DSP_MW_LOG_I("[BLE] play_info->dl_packet_counter %d", 1, play_info->dl_packet_counter);
    DSP_MW_LOG_I("[BLE] play_info->ul_timestamp %d", 1, play_info->ul_timestamp);
    DSP_MW_LOG_I("[BLE] play_info->ISOAnchorClk %d", 1, play_info->ISOAnchorClk);
    DSP_MW_LOG_I("[BLE] play_info->ISOAnchorPhase %d", 1, play_info->ISOAnchorPhase);
    if (n9_ble_dl_if.source == NULL) {
        DSP_MW_LOG_W("[BLE] DL already close ble flow", 0);
    }
    else
    {
        n9_ble_dl_if.source->param.n9ble.iso_interval = (play_info->iso_interval >>1)*625;
        n9_ble_dl_if.source->param.n9ble.ret_window_len = ((play_info->iso_interval + 1) >>1)*625;
        n9_ble_dl_if.source->param.n9ble.seq_miss_cnt = 0;
        n9_ble_dl_if.source->param.n9ble.predict_frame_counter = 0;
        if (n9_ble_dl_if.source->param.n9ble.dual_cis_status != DUAL_CIS_WAITING_SUB)
        {
            n9_ble_dl_if.source->param.n9ble.predict_timestamp = play_info->dl_timestamp_clk + (play_info->dl_ft*play_info->iso_interval);
            n9_ble_dl_if.source->param.n9ble.frame_per_iso = n9_ble_dl_if.source->param.n9ble.iso_interval / n9_ble_dl_if.source->param.n9ble.frame_interval;
            DSP_MW_LOG_I("[le audio DSP] frame per iso %d",1,n9_ble_dl_if.source->param.n9ble.frame_per_iso) ;
        }
        ble_trigger_dl_stream(play_info);
    }
    if ((play_info->ul_timestamp|play_info->ul_ft|play_info->ul_packet_counter)&&(n9_ble_ul_if.sink != NULL)) {
        ble_trigger_ul_stream(play_info);
    }
    else{
        DSP_MW_LOG_W("[BLE] Bypass UL play info", 0);
    }

}

/* End BLE CCNI callback function */
#endif //AIR_BT_CODEC_BLE_ENABLED

#ifdef MTK_CM4_PLAYBACK_ENABLE
/* CM4 Playback CCNI callback function */
void CB_CM4_PLAYBACK_OPEN(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("[CM4_PB] Open", 0);


    /* remap to non-cacheable address */
    mcu2dsp_open_param_p open_param;
    open_param = (mcu2dsp_open_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    DSP_REMAP_SHARE_INFO(open_param->stream_in_param.playback.share_info_base_addr, uint32_t);
    memset(&playback_if, 0 ,sizeof(CONNECTION_IF) );
    playback_if.source   = dsp_open_stream_in(open_param);
    playback_if.sink     = dsp_open_stream_out(open_param);
    DSP_Callback_PreloaderConfig(playback_if.pfeature_table);
}


void CB_CM4_PLAYBACK_START(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("[CM4_PB] Start", 0);
    mcu2dsp_start_param_p start_param;
    start_param = (mcu2dsp_start_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);

    dsp_start_stream_in (start_param, playback_if.source);
    dsp_start_stream_out(start_param, playback_if.sink);

    playback_if.transform = TrasformAudio2Audio(playback_if.source, playback_if.sink, playback_if.pfeature_table);
    if (playback_if.transform == NULL)
    {
        DSP_MW_LOG_E("CM4 Playback transform failed", 0);
    }
}


void CB_CM4_PLAYBACK_STOP(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("[CM4_PB] Stop", 0);
    if (playback_if.transform != NULL)
    {
        StreamDSPClose(playback_if.transform->source,playback_if.transform->sink,msg.ccni_message[0]>>16|0x8000);
    }
    playback_if.transform = NULL;
}

void CB_CM4_PLAYBACK_CLOSE(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("[CM4_PB] Close", 0);
    SourceClose(playback_if.source);
    SinkClose(playback_if.sink);
    DSP_PIC_FeatureDeinit(playback_if.pfeature_table);
    memset(&playback_if,0,sizeof(CONNECTION_IF));
}

void CB_CM4_PLAYBACK_SUSPEND(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("[CM4_PB]  SUSPEND\r\n", 0);

    dsp_trigger_suspend(playback_if.source, playback_if.sink);
}


void CB_CM4_PLAYBACK_RESUME(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("[CM4_PB]  RESUME\r\n", 0);

    dsp_trigger_resume(playback_if.source, playback_if.sink);

}
#endif /* MTK_CM4_PLAYBACK_ENABLE */

#ifdef MTK_LINEIN_PLAYBACK_ENABLE
/* CM4 Line-in Playback CCNI callback function */
void CB_CM4_LINEIN_PLAYBACK_OPEN(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("LINEIN PLAYBACK OPEN\r\n", 0);


    /* remap to non-cacheable address */
    mcu2dsp_open_param_p open_param;
    open_param = (mcu2dsp_open_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    //DSP_REMAP_SHARE_INFO(open_param->stream_in_param.playback.share_info_base_addr, uint32_t);
    memset(&linein_playback_if, 0 ,sizeof(CONNECTION_IF) );
    linein_playback_if.pfeature_table = (stream_feature_list_ptr_t)&stream_feature_list_linein;
    linein_playback_if.source   = dsp_open_stream_in(open_param);
    linein_playback_if.sink     = dsp_open_stream_out(open_param);

    linein_playback_if.source->param.audio.linein_scenario_flag = 1;
    linein_playback_if.sink->param.audio.linein_scenario_flag = 1;

#ifdef AIR_AUDIO_HARDWARE_ENABLE
    Source_Audio_BufferInfo_Rst(linein_playback_if.source, 0);
    Sink_Audio_BufferInfo_Rst(linein_playback_if.sink, 0);
#endif /* AIR_AUDIO_HARDWARE_ENABLE */

#if defined(MTK_LINEIN_PEQ_ENABLE) || defined(MTK_LINEIN_INS_ENABLE)
    DSP_Callback_PreloaderConfig((stream_feature_list_ptr_t)linein_playback_if.pfeature_table);
#endif
    DSP_MW_LOG_I("LINEIN PLAYBACK OPEN Finish\r\n", 0);
}

void CB_CM4_LINEIN_PLAYBACK_START(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("LINEIN PLAYBACK START\r\n", 0);
    mcu2dsp_start_param_p start_param;
    start_param = (mcu2dsp_start_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);

    dsp_start_stream_in (start_param, linein_playback_if.source);
    hal_gpt_delay_us(2000);
    dsp_start_stream_out(start_param, linein_playback_if.sink);

#ifndef AIR_BT_CODEC_BLE_ENABLED
    stream_feature_configure_resolution((stream_feature_list_ptr_t)linein_playback_if.pfeature_table, RESOLUTION_32BIT, CONFIG_DECODER);
#endif
    linein_playback_if.transform = TrasformAudio2Audio(linein_playback_if.source, linein_playback_if.sink, linein_playback_if.pfeature_table);
    if (linein_playback_if.transform == NULL)
    {
        DSP_MW_LOG_E("CM4 Line-in Playback transform failed", 0);
    }
}

void CB_CM4_LINEIN_PLAYBACK_STOP(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);

    DSP_MW_LOG_I("LINEIN PLAYBACK STOP\r\n", 0);
    if (linein_playback_if.transform != NULL)
    {
        StreamDSPClose(linein_playback_if.transform->source, linein_playback_if.transform->sink, msg.ccni_message[0]>>16|0x8000);
    }
    linein_playback_if.transform = NULL;
    DSP_MW_LOG_I("LINEIN PLAYBACK STOP Finish\r\n", 0);
}

void CB_CM4_LINEIN_PLAYBACK_CLOSE(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("LINEIN PLAYBACK CLOSE\r\n", 0);

    //to cancel Sink_Audio_FlushBuffer mute flag for issue BTA-9374 Line-in switch to BT no sound
    hal_audio_volume_digital_gain_parameter_t           digital_gain;
    memset(&digital_gain,0,sizeof(hal_audio_volume_digital_gain_parameter_t));
    digital_gain.memory_select = linein_playback_if.sink->param.audio.memory;
    digital_gain.mute_control = HAL_AUDIO_VOLUME_MUTE_FRAMEWORK;
    digital_gain.mute_enable = false;
    digital_gain.is_mute_control = true;
    #ifdef AIR_AUDIO_HARDWARE_ENABLE
    hal_audio_set_value((hal_audio_set_value_parameter_t*)&digital_gain, HAL_AUDIO_SET_VOLUME_HW_DIGITAL_GAIN);
    #endif /* AIR_AUDIO_HARDWARE_ENABLE */

    SourceClose(linein_playback_if.source);
    SinkClose(linein_playback_if.sink);
    DSP_PIC_FeatureDeinit(linein_playback_if.pfeature_table);
    //DSP_MW_LOG_I("feature PIC unload : %d",1,*(linein_playback_if.pfeature_table));
    memset(&linein_playback_if,0,sizeof(CONNECTION_IF));

    DSP_MW_LOG_I("LINEIN PLAYBACK CLOSE Finish\r\n", 0);
}

void CB_CM4_LINEIN_PLAYBACK_SUSPEND(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("LINEIN PLAYBACK SUSPEND\r\n", 0);

    dsp_trigger_suspend(linein_playback_if.source, linein_playback_if.sink);
}

void CB_CM4_LINEIN_PLAYBACK_RESUME(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("LINEIN PLAYBACK RESUME\r\n", 0);

    dsp_trigger_resume(linein_playback_if.source, linein_playback_if.sink);
}

afe_loopback_param_t dsp_afe_loopack;
//extern hal_audio_device_parameter_vow_t vow_control;

void CB_CM4_TRULY_LINEIN_PLAYBACK_OPEN(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    DSP_MW_LOG_I("Cm4 Truly Line-in Playback Open", 0);
    mcu2dsp_open_param_p open_param;
    UNUSED(ack);

#ifdef AB1568_BRING_UP_DSP_DEFAULT_HW_LOOPBACK
    dsp_afe_loopack.in_device = HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL;//HAL_AUDIO_CONTROL_DEVICE_LINE_IN_DUAL;
    dsp_afe_loopack.in_interface = HAL_AUDIO_INTERFACE_1;
    //dsp_afe_loopack.in_misc_parms = 259;
    dsp_afe_loopack.out_device = HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_DUAL;//HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER;
    dsp_afe_loopack.out_interface = HAL_AUDIO_INTERFACE_1;
    //dsp_afe_loopack.out_misc_parms = 1;
    dsp_afe_loopack.sample_rate = 48000;
    dsp_afe_loopack.with_hw_gain = true;
    dsp_afe_loopack.stream_channel = HAL_AUDIO_DIRECT;
    dsp_afe_loopack.format = AFE_PCM_FORMAT_S32_LE;
#else
    open_param = (mcu2dsp_open_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);

    dsp_afe_loopack.in_device = open_param->stream_in_param.afe.audio_device;
    dsp_afe_loopack.in_interface = open_param->stream_in_param.afe.audio_interface;
    dsp_afe_loopack.in_misc_parms.I2sClkSourceType = open_param->stream_in_param.afe.misc_parms;
    dsp_afe_loopack.out_device = open_param->stream_out_param.afe.audio_device;
    dsp_afe_loopack.out_interface = open_param->stream_out_param.afe.audio_interface;
    dsp_afe_loopack.out_misc_parms.I2sClkSourceType = open_param->stream_out_param.afe.misc_parms;
    dsp_afe_loopack.sample_rate = open_param->stream_out_param.afe.sampling_rate;
    dsp_afe_loopack.with_hw_gain = open_param->stream_out_param.afe.hw_gain;
    dsp_afe_loopack.stream_channel = open_param->stream_out_param.afe.stream_channel;
    dsp_afe_loopack.format = open_param->stream_out_param.afe.format;

#endif
#ifdef LINE_IN_PURE_FOR_AMIC_CLASS_G_HQA
        DSP_MW_LOG_I("dsp_afe_loopack.in_device %d, adc_mode %d\r\n",2,dsp_afe_loopack.in_device,open_param->stream_in_param.afe.adc_mode);
        if (dsp_afe_loopack.in_device & HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL){
            dsp_afe_loopack.device_handle_in.analog_mic.adc_parameter.performance = open_param->stream_in_param.afe.performance;
            dsp_afe_loopack.device_handle_in.analog_mic.bias_voltage[0] = open_param->stream_in_param.afe.bias_voltage[0];
            dsp_afe_loopack.device_handle_in.analog_mic.bias_voltage[1] = open_param->stream_in_param.afe.bias_voltage[1];
            dsp_afe_loopack.device_handle_in.analog_mic.bias_voltage[2] = open_param->stream_in_param.afe.bias_voltage[2];
            dsp_afe_loopack.device_handle_in.analog_mic.bias_voltage[3] = open_param->stream_in_param.afe.bias_voltage[3];
            dsp_afe_loopack.device_handle_in.analog_mic.bias_voltage[4] = open_param->stream_in_param.afe.bias_voltage[4];
            dsp_afe_loopack.device_handle_in.analog_mic.bias_select = open_param->stream_in_param.afe.bias_select;
            dsp_afe_loopack.device_handle_in.analog_mic.iir_filter = open_param->stream_in_param.afe.iir_filter[0];
            dsp_afe_loopack.device_handle_in.analog_mic.adc_parameter.adc_mode = open_param->stream_in_param.afe.adc_mode;
            dsp_afe_loopack.device_handle_in.analog_mic.mic_interface = open_param->stream_in_param.afe.audio_interface;
        } else if(dsp_afe_loopack.in_device & HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL){
            dsp_afe_loopack.device_handle_in.digital_mic.bias_voltage[0] = open_param->stream_in_param.afe.bias_voltage[0];
            dsp_afe_loopack.device_handle_in.digital_mic.bias_voltage[1] = open_param->stream_in_param.afe.bias_voltage[1];
            dsp_afe_loopack.device_handle_in.digital_mic.bias_voltage[2] = open_param->stream_in_param.afe.bias_voltage[2];
            dsp_afe_loopack.device_handle_in.digital_mic.bias_voltage[3] = open_param->stream_in_param.afe.bias_voltage[3];
            dsp_afe_loopack.device_handle_in.digital_mic.bias_voltage[4] = open_param->stream_in_param.afe.bias_voltage[4];
            dsp_afe_loopack.device_handle_in.digital_mic.bias_select = open_param->stream_in_param.afe.bias_select;
            dsp_afe_loopack.device_handle_in.digital_mic.iir_filter = open_param->stream_in_param.afe.iir_filter[0];
            dsp_afe_loopack.device_handle_in.digital_mic.mic_interface = open_param->stream_in_param.afe.audio_interface;
            dsp_afe_loopack.device_handle_in.digital_mic.dmic_selection = open_param->stream_in_param.afe.dmic_selection[0];
            DSP_MW_LOG_I("dsp_afe_loopack dmic_selection %d adc_mode %d",2,dsp_afe_loopack.device_handle_in.digital_mic.dmic_selection,
                open_param->stream_in_param.afe.adc_mode);
        } else if(dsp_afe_loopack.in_device & HAL_AUDIO_CONTROL_DEVICE_LINE_IN_DUAL){
            dsp_afe_loopack.device_handle_in.linein.adc_parameter.performance = open_param->stream_in_param.afe.performance;
            dsp_afe_loopack.device_handle_in.linein.bias_voltage[0] = open_param->stream_in_param.afe.bias_voltage[0];
            dsp_afe_loopack.device_handle_in.linein.bias_voltage[1] = open_param->stream_in_param.afe.bias_voltage[1];
            dsp_afe_loopack.device_handle_in.linein.bias_voltage[2] = open_param->stream_in_param.afe.bias_voltage[2];
            dsp_afe_loopack.device_handle_in.linein.bias_voltage[3] = open_param->stream_in_param.afe.bias_voltage[3];
            dsp_afe_loopack.device_handle_in.linein.bias_voltage[4] = open_param->stream_in_param.afe.bias_voltage[4];
            dsp_afe_loopack.device_handle_in.linein.bias_select = open_param->stream_in_param.afe.bias_select;
            dsp_afe_loopack.device_handle_in.linein.iir_filter = open_param->stream_in_param.afe.iir_filter[0];
            dsp_afe_loopack.device_handle_in.linein.adc_parameter.adc_mode = open_param->stream_in_param.afe.adc_mode;
            //DSP_MW_LOG_I("dsp_afe_loopack linein performance %d, bias_voltagea %d %d %d %d %d, bias_select %d, iir_filter %d, adc_mode %d",9,dsp_afe_loopack.device_handle_in.linein.adc_parameter.performance,
            //    dsp_afe_loopack.device_handle_in.linein.bias_voltage[0],dsp_afe_loopack.device_handle_in.linein.bias_voltage[1],dsp_afe_loopack.device_handle_in.linein.bias_voltage[2],dsp_afe_loopack.device_handle_in.linein.bias_voltage[3],
            //    dsp_afe_loopack.device_handle_in.linein.bias_voltage[4],dsp_afe_loopack.device_handle_in.linein.bias_select,dsp_afe_loopack.device_handle_in.linein.iir_filter,dsp_afe_loopack.device_handle_in.linein.adc_parameter.adc_mode);
        } else if((dsp_afe_loopack.in_device == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER) || (dsp_afe_loopack.in_device == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_L) || (dsp_afe_loopack.in_device == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_R)){
            dsp_afe_loopack.device_handle_in.i2s_master.i2s_interface = open_param->stream_in_param.afe.audio_interface;
        }

        dsp_afe_loopack.device_handle_out.dac.with_high_performance = open_param->stream_out_param.afe.performance;
        DSP_MW_LOG_I("dsp_afe_loopack in_device %d in_interface %d in_misc_parms %d out_device %d out_interface %d out_misc_parms %d sample_rate %d with_hw_gain %d stream_channel %d format %d adc_mode %d",11,
        dsp_afe_loopack.in_device,dsp_afe_loopack.in_interface,dsp_afe_loopack.in_misc_parms,dsp_afe_loopack.out_device,dsp_afe_loopack.out_interface,
        dsp_afe_loopack.out_misc_parms,dsp_afe_loopack.sample_rate,dsp_afe_loopack.with_hw_gain,dsp_afe_loopack.stream_channel,dsp_afe_loopack.format,
        dsp_afe_loopack.device_handle_in.analog_mic.adc_parameter.adc_mode);
#endif
    #ifdef AIR_AUDIO_HARDWARE_ENABLE
    afe_set_loopback_enable(true, &dsp_afe_loopack);
    #endif /* AIR_AUDIO_HARDWARE_ENABLE */
#if 0

    memset(&dsp_vow_control,0,sizeof(hal_audio_device_parameter_vow_t));
    dsp_vow_control.audio_device = HAL_AUDIO_CONTROL_DEVICE_VOW;
    dsp_vow_control.dma_irq_threshold = VOW_SRAM_COPY_SIZE;
    dsp_vow_control.snr_threshold = 0x7373;
    dsp_vow_control.alpha_rise = 0x7;
    dsp_vow_control.dmic_selection = HAL_AUDIO_DMIC_GPIO_DMIC0;
    //dsp_vow_control.suspend_mic = true;
    dsp_vow_control.suspend_mic = false;
    dsp_vow_control.input_device = HAL_AUDIO_CONTROL_DEVICE_VOW;
    //dsp_vow_control.mic_selection = HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL;
    dsp_vow_control.mic_selection = HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL;
    dsp_vow_control.bias_select = HAL_AUDIO_BIAS_SELECT_ALL;
    dsp_vow_control.bias_voltage = HAL_AUDIO_BIAS_VOLTAGE_1_85V;
    dsp_vow_control.mic_interface = HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;
    dsp_vow_control.memory_select = HAL_AUDIO_MEMORY_UL_VUL1;
    dsp_vow_control.adc_parameter.performance = AFE_PEROFRMANCE_ULTRA_LOW_POWER_MODE;
    dsp_vow_control.rate = 16000;
    dsp_vow_control.vow_detection_done_entry = dsp_vow_isr_handler;
    //dsp_vow_control.vow_mode = AFE_VOW_PHASE0;
    dsp_vow_control.vow_mode = AFE_VOW_PHASE1;
    dsp_vow_control.vow_with_hpf = true;

    vow_sink(NULL,NULL);
    vow_enable(&dsp_vow_control,NULL);


#endif


}

void CB_CM4_TRULY_LINEIN_PLAYBACK_CLOSE(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    DSP_MW_LOG_I("Cm4 Truly Line-in Playback Close", 0);
    UNUSED(msg);
    UNUSED(ack);
    #ifdef AIR_AUDIO_HARDWARE_ENABLE
    afe_set_loopback_enable(false, &dsp_afe_loopack);
    #endif /* AIR_AUDIO_HARDWARE_ENABLE */
}
/* End CM4 Line-in Playback CCNI callback function */
#endif /* MTK_LINEIN_PLAYBACK_ENABLE */

#ifdef MTK_CM4_RECORD_ENABLE
/* CM4 Record CCNI callback function */
extern bool CM4_Record_air_dump;
extern U8   CM4_Record_air_dump_scenario;
#if defined(AIR_UL_FIX_SAMPLING_RATE_48K) & defined(AIR_FIXED_RATIO_SRC)
src_fixed_ratio_port_t *record_smp_port;
#endif
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
extern bool CM4_Record_leakage_enable;
#endif
#ifdef MTK_USER_TRIGGER_FF_ENABLE
extern bool utff_enable;
#endif
#ifdef MTK_WWE_ENABLE
hal_audio_device_t  wwe_audio_device;
#endif
void CB_CM4_RECORD_OPEN(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("Cm4 record Open", 0);

    /* remap to non-cacheable address */
    mcu2dsp_open_param_p open_param;
    open_param = (mcu2dsp_open_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    memset(&record_if, 0 ,sizeof(CONNECTION_IF) );
    audio_dsp_codec_type_t audio_dsp_codec_type;
    audio_dsp_codec_type = msg.ccni_message[0]&0x0fff;
#ifdef MTK_WWE_ENABLE
    g_wwe_mode = ((msg.ccni_message[0]&0xf000) >> 12);
    DSP_MW_LOG_I("Cm4 record Open wwe_mode = %d", 1, g_wwe_mode);
    wwe_audio_device = open_param->stream_in_param.afe.audio_device;
#endif
    DSP_MW_LOG_I("Cm4 record Open msg.ccni_message[0]:0x%x", 1,msg.ccni_message[0]);
    DSP_MW_LOG_I("Cm4 record Open codec type:0x%3x", 1,audio_dsp_codec_type);
    switch(audio_dsp_codec_type){
#ifdef MTK_RECORD_OPUS_ENABLE
        case AUDIO_DSP_CODEC_TYPE_OPUS:{
            record_if.pfeature_table = (stream_feature_list_ptr_t)&stream_feature_list_opus_mic_record;
            break;
        }
#endif
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
        case AUDIO_DSP_CODEC_TYPE_ANC_LC:{
            CM4_Record_leakage_enable = true;
            record_if.pfeature_table = (stream_feature_list_ptr_t)&stream_feature_list_leakage_compensation;
            break;
        }
#endif
#ifdef MTK_USER_TRIGGER_FF_ENABLE
        case AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF_SZ:{
            utff_enable = true;
            record_if.pfeature_table = (stream_feature_list_ptr_t)&stream_feature_list_user_trigger_adaptive_ff_sz;
            DSP_MW_LOG_I("[user_trigger_ff]CB_CM4_RECORD_OPEN, codec type:0x%x", 1, audio_dsp_codec_type);
            DSP_MW_LOG_I("[user_trigger_ff]iir_filter[0]:0x%x, iir_filter[1]:0x%x, iir_filter[2]:0x%x", 3, open_param->stream_in_param.afe.iir_filter[0], open_param->stream_in_param.afe.iir_filter[1], open_param->stream_in_param.afe.iir_filter[2]);
            break;
        }
        case AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF_PZ:{
            utff_enable = true;
            record_if.pfeature_table = (stream_feature_list_ptr_t)&stream_feature_list_user_trigger_adaptive_ff_pz;
            DSP_MW_LOG_I("[user_trigger_ff]CB_CM4_RECORD_OPEN, codec type:0x%x", 1, audio_dsp_codec_type);
            DSP_MW_LOG_I("[user_trigger_ff]iir_filter[0]:0x%x, iir_filter[1]:0x%x, iir_filter[2]:0x%x", 3, open_param->stream_in_param.afe.iir_filter[0], open_param->stream_in_param.afe.iir_filter[1], open_param->stream_in_param.afe.iir_filter[2]);
            break;
        }
        case AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF_PZ_FIR:{
            utff_enable = true;
            record_if.pfeature_table = (stream_feature_list_ptr_t)&stream_feature_list_user_trigger_adaptive_ff_pz_fir;
            DSP_MW_LOG_I("[user_trigger_ff]CB_CM4_RECORD_OPEN, codec type:0x%x", 1, audio_dsp_codec_type);
            DSP_MW_LOG_I("[user_trigger_ff]iir_filter[0]:0x%x, iir_filter[1]:0x%x, iir_filter[2]:0x%x", 3, open_param->stream_in_param.afe.iir_filter[0], open_param->stream_in_param.afe.iir_filter[1], open_param->stream_in_param.afe.iir_filter[2]);
            break;
        }
#endif
        case AUDIO_DSP_CODEC_TYPE_PCM:{
            record_if.pfeature_table = (stream_feature_list_ptr_t)&stream_feature_list_mic_record;
            break;
        }
        default:
            if((audio_dsp_codec_type & 0xff00) == 0xff00){ /*Record air dump.*/
                CM4_Record_air_dump = true;
                CM4_Record_air_dump_scenario = audio_dsp_codec_type & 0x000f;
                record_if.pfeature_table = (stream_feature_list_ptr_t)&stream_feature_list_mic_record_airdump;
            } else {
                record_if.pfeature_table = (stream_feature_list_ptr_t)&stream_feature_list_mic_record;
            }
        break;
    }
#ifdef MTK_WWE_ENABLE
    if(g_wwe_mode != WWE_MODE_NONE)
    {
        record_if.pfeature_table = (stream_feature_list_ptr_t)&stream_feature_list_wwe_mic_record;
        g_mcu2dsp_vad_param = (mcu2dsp_vad_param_p)open_param->stream_out_param.record.p_share_info->startaddr;

        DSP_Callback_PreloaderConfig(record_if.pfeature_table);
    }
#endif
    record_if.source   = dsp_open_stream_in(open_param);
    record_if.sink     = dsp_open_stream_out(open_param);
#ifdef MTK_WWE_ENABLE
    wwe_processing_init();
#endif
#if defined(AIR_UL_FIX_SAMPLING_RATE_48K) & defined(AIR_FIXED_RATIO_SRC)
    switch(audio_dsp_codec_type) {
        // #ifdef MTK_LEAKAGE_DETECTION_ENABLE
        case AUDIO_DSP_CODEC_TYPE_ANC_LC:
        case AUDIO_DSP_CODEC_TYPE_PCM:
        {
            src_fixed_ratio_config_t smp_config;
            smp_config.channel_number = 0;
            smp_config.in_sampling_rate = 48000;
            smp_config.out_sampling_rate = 16000;
            smp_config.resolution = RESOLUTION_16BIT;
            smp_config.multi_cvt_mode = SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_SINGLE;
            smp_config.cvt_num = 1;
            record_smp_port = stream_function_src_fixed_ratio_get_port(record_if.source);
            stream_function_src_fixed_ratio_init(record_smp_port, &smp_config);
            break;
        }
        // #endif
        default:
            break;
    }
#endif
}


void CB_CM4_RECORD_START(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("Cm4 record Start", 0);
    mcu2dsp_start_param_p start_param;
    start_param = (mcu2dsp_start_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);

    dsp_start_stream_in (start_param, record_if.source);
    dsp_start_stream_out(start_param, record_if.sink);

    record_if.source->param.audio.AfeBlkControl.u4awsflag = 1;
    record_if.transform = TrasformAudio2Audio(record_if.source, record_if.sink, record_if.pfeature_table);
    hal_audio_trigger_start_parameter_t sw_trigger_start;
    sw_trigger_start.enable = true;
    sw_trigger_start.memory_select = record_if.source->param.audio.mem_handle.memory_select;
    hal_audio_set_value((hal_audio_set_value_parameter_t *)&sw_trigger_start,HAL_AUDIO_SET_TRIGGER_MEMORY_START);
}


void CB_CM4_RECORD_STOP(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("Cm4 record Stop", 0);
#ifdef MTK_WWE_ENABLE
     /*must deinit hwvad first to resume mic ,then stop the record*/
    wwe_hwvad_deinit();
#endif
    if (record_if.transform != NULL)
    {
        StreamDSPClose(record_if.transform->source,record_if.transform->sink,msg.ccni_message[0]>>16|0x8000);
    }else{
        DSP_MW_LOG_E("Cm4 record not exit, just ack.", 0);
        aud_msg_ack(msg.ccni_message[0]>>16|0x8000, FALSE);
    }
    record_if.transform = NULL;
}

void CB_CM4_RECORD_CLOSE(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("Cm4 record Close", 0);
#ifdef MTK_WWE_ENABLE
    wwe_processing_deinit();
#endif
#if defined(AIR_UL_FIX_SAMPLING_RATE_48K) & defined(AIR_FIXED_RATIO_SRC)
    if (record_smp_port) {
        stream_function_src_fixed_ratio_deinit(record_smp_port);
        record_smp_port = NULL;
    }
#endif
    SourceClose(record_if.source);
    SinkClose(record_if.sink);
    DSP_PIC_FeatureDeinit(record_if.pfeature_table);
    memset(&record_if,0,sizeof(CONNECTION_IF));
    CM4_Record_air_dump = false;
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
    CM4_Record_leakage_enable = false;
#endif
#ifdef MTK_USER_TRIGGER_FF_ENABLE
utff_enable = false;
#endif
}

void CB_CM4_RECORD_SUSPEND(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("Cm4 recordSUSPEND\r\n", 0);
    dsp_trigger_suspend(record_if.source, record_if.sink);

}

void CB_CM4_RECORD_RESUME(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("Cm4 record RESUME\r\n", 0);
#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
    dsp_detachable_config(&record_if,msg);
#endif
    dsp_trigger_resume(record_if.source, record_if.sink);
}
/* End Playback CCNI callback function */

#ifdef MTK_LEAKAGE_DETECTION_ENABLE
void CB_CM4_RECORD_LC_SET_PARAM_ACK(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    anc_leakage_compensation_parameters_nvdm_p leakage_compensation_param;
    leakage_compensation_param = (anc_leakage_compensation_parameters_nvdm_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    extern anc_leakage_compensation_parameters_nvdm_t leakage_compensation_thd;
    leakage_compensation_thd.ld_thrd = leakage_compensation_param->ld_thrd;
    leakage_compensation_thd.RXIN_TXREF_DELAY = leakage_compensation_param->RXIN_TXREF_DELAY;
    leakage_compensation_thd.DIGITAL_GAIN = leakage_compensation_param->DIGITAL_GAIN;
    leakage_detection_set_duration(leakage_compensation_param->report_thd, leakage_compensation_param->no_response_thd);
    DSP_MW_LOG_I("[RECORD_LC]CB_CM4_RECORD_LC_SET_PARAM_ACK, ld_thrd:0x%x, RXIN_TXREF_DELAY:0x%x, DIGITAL_GAIN:0x%x, report_thd:%u, no_response_thd:%u\r\n", 5, leakage_compensation_param->ld_thrd, leakage_compensation_param->RXIN_TXREF_DELAY, leakage_compensation_param->DIGITAL_GAIN, leakage_compensation_param->report_thd, leakage_compensation_param->no_response_thd);
}
#endif
#endif /* MTK_CM4_RECORD_ENABLE */

#ifdef AIR_SIDETONE_ENABLE
afe_sidetone_param_t dsp_afe_sidetone;
void dsp_sidetone_start(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);

    U32 gpt_timer;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_timer);
    DSP_MW_LOG_I("SideTone Start gpt : %d\r\n", 1,gpt_timer);

    mcu2dsp_sidetone_param_p start_param;
    //mcu2dsp_sidetone_param_t sidetone;


    #if 0
    dsp_afe_sidetone.in_device      = HAL_AUDIO_DEVICE_MAIN_MIC_DUAL;
    dsp_afe_sidetone.in_interface   = HAL_AUDIO_INTERFACE_1;
    dsp_afe_sidetone.out_device     = HAL_AUDIO_DEVICE_DAC_DUAL;
    dsp_afe_sidetone.out_interface  = HAL_AUDIO_INTERFACE_1;
    dsp_afe_sidetone.channel        = HAL_AUDIO_DIRECT;
    dsp_afe_sidetone.gain           = 600;
    #else
    start_param = (mcu2dsp_sidetone_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    memcpy(&dsp_afe_sidetone, start_param, sizeof(mcu2dsp_sidetone_param_t));
    #endif
    //afe_set_sidetone_enable(true, dsp_afe_sidetone);

    #ifdef AIR_AUDIO_HARDWARE_ENABLE
    afe_set_sidetone_enable_flag(true, dsp_afe_sidetone.gain);
    #endif /* AIR_AUDIO_HARDWARE_ENABLE */

    DSP_MW_LOG_I("SideTone Start gain %d\r\n", 1,dsp_afe_sidetone.gain);
    DTM_enqueue(DTM_EVENT_ID_SIDETONE_START, 0, false);
}

void dsp_sidetone_stop(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("SideTone Stop\r\n", 0);
    #if 0
    mcu2dsp_sidetone_param_p start_param;
    mcu2dsp_sidetone_param_t sidetone;
    start_param = (mcu2dsp_sidetone_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    memcpy(&sidetone, start_param, sizeof(mcu2dsp_sidetone_param_t));
    #endif
    //afe_set_sidetone_enable(false, dsp_afe_sidetone);
    #ifdef AIR_AUDIO_HARDWARE_ENABLE
    afe_set_sidetone_enable_flag(false, dsp_afe_sidetone.gain);
    #endif /* AIR_AUDIO_HARDWARE_ENABLE */
    #if 0//modify for ab1568
#ifdef ENABLE_SIDETONE_RAMP_TIMER
    fw_sidetone_set_ramp_timer(FW_SIDETONE_MUTE_GAIN);
#else
    DTM_enqueue(DTM_EVENT_ID_SIDETONE_STOP, 0, false);
#endif
    #else
    DTM_enqueue(DTM_EVENT_ID_SIDETONE_STOP_RAMP, 0, false);
    //hal_audio_set_device(&(dsp_afe_sidetone.device_handle_in_side_tone), dsp_afe_sidetone.device_handle_in_side_tone.sidetone.audio_device, HAL_AUDIO_CONTROL_OFF);
    DSP_MW_LOG_I("sidetone device 0x%x off\r\n",1,dsp_afe_sidetone.device_handle_in_side_tone.sidetone.audio_device);
    #endif
}

void dsp_sidetone_set_volume(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("SideTone Set_volume\r\n", 0);
    int32_t sidetone_gain = (int32_t)msg.ccni_message[1];
    #ifdef AIR_AUDIO_HARDWARE_ENABLE
    afe_set_sidetone_volume(sidetone_gain);
    #endif /* AIR_AUDIO_HARDWARE_ENABLE */
}

void dsp_sidetone_start_volume_set(void)
{
    if (afe_get_sidetone_enable_flag() == true)
    {
        //hal_gpt_delay_ms(200);
        vTaskDelay(pdMS_TO_TICKS(200));
        #ifdef AIR_AUDIO_HARDWARE_ENABLE
        afe_set_sidetone_volume(afe_get_sidetone_gain());
        #endif /* AIR_AUDIO_HARDWARE_ENABLE */
    }
}
#endif

void dsp_dc_compensation_start(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
#ifdef AIR_AUDIO_HARDWARE_ENABLE
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("[DC COMPENSATION] Start\r\n", 0);
    hal_audio_device_parameter_t device_handle;
    hal_audio_set_value_parameter_t set_val_handle;
    memset(&device_handle, 0, sizeof(hal_audio_device_parameter_t));
    memset(&set_val_handle, 0, sizeof(hal_audio_set_value_parameter_t));
    mcu2dsp_open_param_p open_param;
    open_param = (mcu2dsp_open_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    //hal_audio_device_t device = open_param->stream_out_param.afe.audio_device;
    bool with_high_performance = open_param->stream_out_param.afe.performance;
    hal_audio_analog_mdoe_t dac_mode = open_param->stream_out_param.afe.dl_dac_mode;
    set_val_handle.analog_output_mode.dac_mode = dac_mode;
    set_val_handle.analog_output_mode.gain_select = AFE_HW_ANALOG_GAIN_OUTPUT;
    hal_audio_set_value(&set_val_handle, HAL_AUDIO_SET_VOLUME_OUTPUT_ANALOG_SETTING);
    if ((set_val_handle.analog_output_mode.dac_mode == HAL_AUDIO_ANALOG_OUTPUT_CLASSAB) ||
        (set_val_handle.analog_output_mode.dac_mode == HAL_AUDIO_ANALOG_OUTPUT_CLASSG)) {
        device_handle.dac.rate = 48000;
        device_handle.dac.dac_mode= dac_mode;

        device_handle.dac.dc_compensation_value = afe.stream_out.dc_compensation_value;
        device_handle.dac.with_high_performance = with_high_performance;
        device_handle.dac.with_force_change_rate = false;

        hal_audio_set_device(&device_handle, HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_DUAL, HAL_AUDIO_CONTROL_ON);

        AFE_WRITE(0xC0000EDC, 0x11004089);              // Setting voltage high to 0.9v
        #if 1//modfiy for ab1568
        //ANA_SET_REG (0xA207012C, 0x1<<2, 0x1<<2); // Enable trim buffer's reference voltage
        ANA_SET_REG (AUDDEC_ANA_CON11, 0x1<<2, 0x1<<2); // Enable trim buffer's reference voltage
        DSP_MW_LOG_I("AUDDEC_ANA_CON11 addr:0x%x val:0x%x\r\n", 2, AUDDEC_ANA_CON11, AFE_READ(AUDDEC_ANA_CON11));
        #endif
    } else {
        device_handle.analog_mic.audio_device = HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL;
        device_handle.analog_mic.rate = 48000;
        device_handle.analog_mic.mic_interface = HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;
        device_handle.analog_mic.bias_select = HAL_AUDIO_BIAS_SELECT_BIAS0;
        device_handle.analog_mic.bias_voltage[0] = HAL_AUDIO_BIAS_VOLTAGE_1_85V;
        device_handle.analog_mic.iir_filter = false;
        device_handle.analog_mic.with_external_bias = false;
        device_handle.analog_mic.with_bias_lowpower = false;
        device_handle.analog_mic.with_phase_inverse = false;
        device_handle.analog_mic.adc_parameter.adc_mode = HAL_AUDIO_ANALOG_INPUT_ACC10K;              //AU_AFE_IN_MODE_t
        device_handle.analog_mic.adc_parameter.performance = AFE_PEROFRMANCE_NORMAL_MODE;  //AU_AFE_POWER_SETTING_t
        device_handle.analog_mic.adc_parameter.is_fifo_sync_clock_ch1 = false;
        device_handle.analog_mic.adc_parameter.with_au_in_swap = false;
        hal_audio_set_device(&device_handle, device_handle.analog_mic.audio_device, HAL_AUDIO_CONTROL_ON);
    }
//#endif
#else
    UNUSED(msg);
    UNUSED(ack);
#endif /* AIR_AUDIO_HARDWARE_ENABLE */
}

void dsp_dc_compensation_stop(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
#ifdef AIR_AUDIO_HARDWARE_ENABLE
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("[DC COMPENSATION] Stop\r\n", 0);
    afe.stream_out.dc_compensation_value = (uint16_t)msg.ccni_message[0];
    hal_audio_device_parameter_t device_handle;
    memset(&device_handle, 0, sizeof(hal_audio_device_parameter_t));
    mcu2dsp_open_param_p open_param;
    open_param = (mcu2dsp_open_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    //hal_audio_device_t device = open_param->stream_out_param.afe.audio_device;
    bool with_high_performance = open_param->stream_out_param.afe.performance;
    hal_audio_analog_mdoe_t dac_mode = open_param->stream_out_param.afe.dl_dac_mode;
    bool no_delay_flag = open_param->stream_out_param.afe.misc_parms; // just reuse this parameter
        afe.stream_out.dac_mode = dac_mode;
    if ((dac_mode == HAL_AUDIO_ANALOG_OUTPUT_CLASSAB) ||
        (dac_mode == HAL_AUDIO_ANALOG_OUTPUT_CLASSG)) {
            ANA_WRITE(AUDDEC_ANA_CON6, 0x0070);
            //ANA_SET_REG (AUDDEC_ANA_CON10, 0, 0x1<<2);  // Disable trim buffer's reference voltage
            ANA_SET_REG (AUDDEC_ANA_CON11, 0, 0x1<<2);  // Disable trim buffer's reference voltage
            ANA_WRITE(AUDDEC_ANA_CON6, 0x0030);

            AFE_WRITE(0xC0000EDC, 0x11004001);          // Setting voltage low to 0.42v
            device_handle.dac.rate = 48000;
        device_handle.dac.dac_mode= dac_mode;
        device_handle.dac.with_high_performance = with_high_performance;
            device_handle.dac.dc_compensation_value = afe.stream_out.dc_compensation_value;
            device_handle.dac.with_force_change_rate = false;
            hal_audio_set_device(&device_handle, HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_DUAL, HAL_AUDIO_CONTROL_OFF);
        }else{
            hal_audio_analog_mdoe_t dl_analog_mode = HAL_AUDIO_ANALOG_OUTPUT_CLASSD;
            device_handle.analog_mic.audio_device = HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL;
            device_handle.analog_mic.rate = 48000;
            device_handle.analog_mic.mic_interface = HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;
            device_handle.analog_mic.bias_select = HAL_AUDIO_BIAS_SELECT_BIAS0;
            device_handle.analog_mic.bias_voltage[0] = HAL_AUDIO_BIAS_VOLTAGE_1_85V;
            device_handle.analog_mic.iir_filter = false;
            device_handle.analog_mic.with_external_bias = false;
            device_handle.analog_mic.with_bias_lowpower = false;
            device_handle.analog_mic.with_phase_inverse = false;
            device_handle.analog_mic.adc_parameter.adc_mode = HAL_AUDIO_ANALOG_INPUT_ACC10K;              //AU_AFE_IN_MODE_t
            device_handle.analog_mic.adc_parameter.performance = AFE_PEROFRMANCE_NORMAL_MODE;//AU_AFE_POWER_SETTING_t
            device_handle.analog_mic.adc_parameter.is_fifo_sync_clock_ch1 = false;
            device_handle.analog_mic.adc_parameter.with_au_in_swap = false;
            afe.stream_out.dc_compensation_value = hal_audio_get_value((hal_audio_get_value_parameter_t *)&dl_analog_mode, HAL_AUDIO_GET_DEVICE_DAC_COMPENSATION_VALUE);
            hal_audio_set_device(&device_handle, device_handle.analog_mic.audio_device, HAL_AUDIO_CONTROL_OFF);
        }
//#endif

    DSP_MW_LOG_I("DC COMPENSATION val:0x%x, no_delay_flag = 0x%x\r\n", 2,afe.stream_out.dc_compensation_value, no_delay_flag);

    uint32_t value = 0;
    if(no_delay_flag != 0) {
        value = 0;
    } else {
        value = 2000;
    }
    hal_audio_set_value((hal_audio_set_value_parameter_t *)&value, HAL_AUDIO_SET_DEVICE_AMP_DELAY_TIMER_MS);
#else
    UNUSED(msg);
    UNUSED(ack);
#endif /* AIR_AUDIO_HARDWARE_ENABLE */
}

#ifdef MTK_BT_A2DP_ENABLE
void dsp_alc_switch(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("a2dp alc switch :%d\r\n", 1,msg.ccni_message[1]);
    #ifdef AIR_AUDIO_HARDWARE_ENABLE
    Audio_setting->Audio_sink.alc_enable = (bool)(uint32_t)(msg.ccni_message[1]);
    #endif /* AIR_AUDIO_HARDWARE_ENABLE */
}
#endif

#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
void dsp_peq_set_param(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    BOOL BypassTimestamp = FALSE;
    mcu2dsp_peq_param_p peq_param;
    peq_param = (mcu2dsp_peq_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);

    #ifdef MTK_CELT_DEC_ENABLE
    DSP_STREAMING_PARA_PTR  pStream;
    if(peq_param->setting_mode == PEQ_SYNC){
        if(n9_a2dp_if.source != NULL && n9_a2dp_if.sink != NULL){
            pStream = DSP_Streaming_Get(n9_a2dp_if.source,n9_a2dp_if.sink);
            if(pStream == NULL){
                BypassTimestamp = FALSE;
            }
            else if(pStream->callback.FeatureTablePtr->ProcessEntry == stream_codec_decoder_celt_process){
                BypassTimestamp = TRUE;
            }
        }
    }
    #endif

    DSP_MW_LOG_I("PEQ set param with phase %d\r\n", 1, peq_param->phase_id);
    PEQ_Set_Param(msg, ack, BypassTimestamp);
    if((msg.ccni_message[0] & 0xFFFF) == 0) {
        Audio_CPD_Enable(peq_param->drc_enable, peq_param->phase_id, peq_param->drc_force_disable,0,peq_param->peq_nvkey_id);
    }
    else{
        Audio_CPD_Enable(peq_param->drc_enable, peq_param->phase_id, peq_param->drc_force_disable,1,peq_param->peq_nvkey_id);
    }
}
#endif
#ifdef MTK_GAMING_MODE_HEADSET

#ifdef TRIGGER_AIR_A2DP_PROCSESS_TIMER
static void trigger_a2dp_airoha_proc_timer_callback(void *user_data)
{
    UNUSED(user_data);
//    hal_gpio_set_output(HAL_GPIO_9, 1);//debug for play en decode 1st frame
    U32 bt_clk_cur;
    U16 intra_clk_cur;
    MCE_GetBtClk((BTCLK *)&bt_clk_cur,(BTPHASE *)&intra_clk_cur,ULL_CLK_Offset);
    DSP_MW_LOG_I("[AiroGaming]Play En Timer Callback Decode 1st Frame,Clk:0x%x,phase:0x%x",2,bt_clk_cur,intra_clk_cur);
    if ((n9_a2dp_if.source != NULL)&&(n9_a2dp_if.sink != NULL)&&(n9_a2dp_if.transform != NULL)&&(n9_a2dp_if.sink->param.audio.irq_exist == FALSE))
    {
        n9_a2dp_if.source->transform->Handler(n9_a2dp_if.source, n9_a2dp_if.sink);
        if (n9_a2dp_if.source->transform->sink->taskid == DPR_TASK_ID) {
            xTaskResumeFromISR(pDPR_TaskHandler);
        } else if (n9_a2dp_if.source->transform->sink->taskid == DHP_TASK_ID) {
            xTaskResumeFromISR(pDHP_TaskHandler);
        } else {
            xTaskResumeFromISR(pDAV_TaskHandler);
        }
    } else {
        DSP_MW_LOG_W("[AiroGaming] Trigger 1ST Decode faill", 0);
    }
    hal_gpt_sw_free_timer(g_play_en_gpt_handler);
    g_play_en_gpt_handler = NULL;
//    hal_gpio_set_output(HAL_GPIO_9, 0);//debug for play en decode 1st frame
}
#endif

void dsp_a2dp_set_dl_play_en(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    BTCLK bt_clk;
    BTPHASE bt_phase;
    audio_dsp_a2dp_dl_play_en_param_p play_en_param;

    play_en_param = (audio_dsp_a2dp_dl_play_en_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    if(n9_a2dp_if.sink->param.audio.channel_num>=2){
        n9_a2dp_if.sink->streamBuffer.BufferInfo.WriteOffset = ((120<<2)<<1)<<1;
    }else{
        n9_a2dp_if.sink->streamBuffer.BufferInfo.WriteOffset = (120<<2)<<1;
    }
    AFE_SET_REG(AFE_MEMIF_PBUF_SIZE, 0, 3<<0);//reduce pbuffer size
    AFE_SET_REG(AFE_MEMIF_MINLEN, 1, 0XF);//reduce pbuffer size

    DSP_MW_LOG_I("A2DP DL - Play En SeqNo:%d, BtClk:0x%x", 2, play_en_param->sequence_number, play_en_param->bt_clock);

    audio_headset_ull_ctrl.play_en.sequence_number = play_en_param->sequence_number;
    audio_headset_ull_ctrl.play_en.bt_clock = ((play_en_param->bt_clock + 22 - 2 /* 6.5ms DL reserve time */) & BTCLK_27_0_MASK);

     //================debug test===================
    BTCLK bt_clk_1,bt_clk_2,bt_clk_3,bt_clk_4;
    BTPHASE bt_phase_1,bt_phase_2,bt_phase_3,bt_phase_4;
    MCE_Get_BtClkOffset(&bt_clk_1, &bt_phase_1,ULL_CLK_Offset);
    MCE_Get_BtClkOffset(&bt_clk_2, &bt_phase_2,BT_CLK_Offset);

    MCE_GetBtClk(&bt_clk_3, &bt_phase_3,ULL_CLK_Offset);
    MCE_GetBtClk(&bt_clk_4, &bt_phase_4,BT_CLK_Offset);
    if((bt_clk_1 != bt_clk_2) && (rBb_ull->rValid)){
        DSP_MW_LOG_E("[MCE]ULL_CLK_Offset != BT_CLK_Offset , ULL_CLK:0x%x, BT_CLK:0x%x",2,bt_clk_3,bt_clk_4);
        assert(0);
    }else{
        DSP_MW_LOG_I("[MCE]ULL_CLK_Offset != BT_CLK_Offset , ULL_CLK:0x%x, BT_CLK:0x%x",2,bt_clk_3,bt_clk_4);
    }
    //==========================================

    if (audio_headset_ull_ctrl.is_a2dp_started) {

        MCE_GetBtClk(&bt_clk, &bt_phase,ULL_CLK_Offset);
        audio_headset_ull_ctrl.play_en.bt_clock +=  ((n9_a2dp_if.source->param.n9_a2dp.sink_latency - ULL_BASIC_LATENCY)*8/2500);/* 8/2500 = 312.5 for frame based  */

        DSP_MW_LOG_I("A2DP DL - A2DP Start First, Current BtClk:0x%x, Phase:0x%x", 2, bt_clk,bt_phase);

        a2dp_ull_get_proper_play_en(audio_headset_ull_ctrl.play_en.bt_clock,
                                    audio_headset_ull_ctrl.play_en.sequence_number,
                                    &audio_headset_ull_ctrl.play_en.bt_clock,
                                    (U8*)&audio_headset_ull_ctrl.play_en.sequence_number);
        audio_headset_ull_ctrl.play_en.sequence_number++;
        a2dp_ull_set_init_seq_no(audio_headset_ull_ctrl.play_en.sequence_number);

        MCE_TransBT2NativeClk(audio_headset_ull_ctrl.play_en.bt_clock,625/*625 1 bt clk,22-1+1 */, &bt_clk, &bt_phase,ULL_CLK_Offset);

        hal_audio_afe_set_play_en(bt_clk, bt_phase);
        DSP_MW_LOG_I("[AiroGaming]AFE Set Val: bt_clk:0x%x, bt_phase:0x%x", 2, bt_clk, bt_phase);

        #ifdef TRIGGER_AIR_A2DP_PROCSESS_TIMER
        U32 bt_clk_cur;
        U16 intra_clk_cur;

        if(g_play_en_gpt_handler == NULL){
            if (hal_gpt_sw_get_timer((uint32_t *)&g_play_en_gpt_handler) != HAL_GPT_STATUS_OK) {
                DSP_MW_LOG_W("[AiroGaming]play en get timer fail",0);
                assert(0);
            }else{
                DSP_MW_LOG_I("[AiroGaming]play en get timer success",0);
            }
        }
        MCE_GetBtClk((BTCLK *)&bt_clk_cur,(BTPHASE *)&intra_clk_cur,ULL_CLK_Offset);
        U32 FirstDecodeCnt = ((((audio_headset_ull_ctrl.play_en.bt_clock)-(bt_clk_cur)) >> 1)*625 - (intra_clk_cur >>1) - 625);
        hal_gpt_sw_start_timer_us(g_play_en_gpt_handler,FirstDecodeCnt,trigger_a2dp_airoha_proc_timer_callback,NULL);
        DSP_MW_LOG_I("[AiroGaming]bclk_play_en:0x%x, bclk_cur:0x%x, timer_duration:%d us", 3, audio_headset_ull_ctrl.play_en.bt_clock, bt_clk_cur, FirstDecodeCnt);
        #endif

    } else {
        DSP_MW_LOG_I("A2DP DL - A2DP Play En First", 0);
    }

    audio_headset_ull_ctrl.is_play_en_ready = TRUE;

    UNUSED(ack);
}
#endif

void dsp_set_sysram_addr(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("DSP NVDM init\r\n", 0);

    U32 sysram_start_addr;

    sysram_start_addr = hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    #if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE) || defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR)
    dsp0_nvkey_init(sysram_start_addr, 1024*3+512);// SysRam fixed 3k size
    #else
    dsp0_nvkey_init(sysram_start_addr, 1024*3);// SysRam fixed 3k size
    #endif
}


void dsp_streaming_deinit_all(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    bool enable = (bool)(uint16_t)msg.ccni_message[1];
    audio_stream_deinit_id_t stream_deinit = (audio_stream_deinit_id_t)(msg.ccni_message[0]&0xFFFF);
    DSP_MW_LOG_I("DSP stream deinit,mute status :%d, ID:%d \r\n", 2, enable, stream_deinit);
    if (stream_deinit == AUDIO_STRAM_DEINIT_VOICE_AEC) {
#ifdef AIR_VOICE_NR_ENABLE
        stream_function_aec_nr_deinitialize(true);
#endif
    } else if (stream_deinit == AUDIO_STRAM_DEINIT_ANC_MONITOR) {
#ifdef AIR_ANC_WIND_DETECTION_ENABLE
        stream_function_wind_reset_status();
#endif
#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
        stream_function_environment_detection_reset_status();
#endif
    } else {
        DAVT_StreeamingDeinitAll();
        DHPT_StreeamingDeinitAll();
        #if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
        DLLT_StreeamingDeinitAll();
        #endif
        //hal_audio_mute_stream_out(enable);
#ifdef AIR_VOICE_NR_ENABLE
       stream_function_aec_nr_deinitialize(false);
#ifdef MTK_3RD_PARTY_NR
        stream_function_tx_eq_deinitialize();
#endif
#endif
#ifdef MTK_BT_HFP_SPE_ALG_V2
#if defined(AIR_DRC_ENABLE) || defined(AIR_VOICE_DRC_ENABLE)
        stream_function_cpd_deinitialize();
#endif
#endif
#ifdef MTK_LINEIN_INS_ENABLE
        stream_function_ins_deinitialize();
#endif

    }
}

#ifdef AIR_VOICE_NR_ENABLE
#ifdef MTK_AIRDUMP_EN
void CB_CM4_SCO_AIRDUMP_EN(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    U32 mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    if(msg.ccni_message[1] == 1)
    {
        DSP_MW_LOG_I("[AirDump][DSP] AIRDUMP Start\r\n", 0);
        AEC_NR_AirDumpEnable(TRUE);
    }
    else if(msg.ccni_message[1] == 0)
    {
        DSP_MW_LOG_I("[AirDump][DSP] AIRDUMP Stop\r\n", 0);
        AEC_NR_AirDumpEnable(FALSE);
    }
    hal_nvic_restore_interrupt_mask(mask);
}
#endif

void CB_CM4_SCO_DL_AVC_VOL_UPDATE(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("[NDVC][DSP receive] avc_vol: %d, gfgAvcUpdate: %d",2, msg.ccni_message[1], gfgAvcUpdate);
    if(gfgAvcUpdate == false){
       gfgAvcUpdate = true;
       gi2AvcVol = msg.ccni_message[1];
    }
}

void dsp_get_reference_gain(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("DSP get reference gain\r\n", 0);
    S16* Refgain_start_addr;
    Refgain_start_addr = (S16*)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    AEC_NR_GetRefGain(Refgain_start_addr);
}
#endif

typedef struct {
    uint32_t type;
    uint32_t length;
    uint8_t version[32];
} race_dsprealtime_query_lib_version_noti_struct_t;

void dsp_query_lib_version(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    race_dsprealtime_query_lib_version_noti_struct_t *ver_struct;

    UNUSED(ack);

    ver_struct = (race_dsprealtime_query_lib_version_noti_struct_t *)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    if (ver_struct->type == DSP_ALG_LIB_TYPE_IGO) {
#ifdef AIR_VOICE_NR_ENABLE
        ver_struct->length = AEC_NR_Query_Lib_Version(ver_struct->type, ver_struct->version);
#else
        ver_struct->length = 0;
#endif
    } else {
        ver_struct->length = 0;
        DSP_MW_LOG_I("DSP query lib version fail, type %d\r\n", 1, ver_struct->type);
        return;
    }
    
    DSP_MW_LOG_I("DSP query lib version success\r\n", 0);
}

void dsp_set_algorithm_param(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    void *share_ptr = (void *)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    uint16_t nvkey_id = (uint16_t)(msg.ccni_message[0]&0xFFFF);
    DSP_MW_LOG_I("DSP set algorithm parameters nvkey_ID:0x%x, ptr:0x%x", 2, nvkey_id ,share_ptr);

    if (0) {
#ifdef AIR_ANC_WIND_DETECTION_ENABLE
    } else if (nvkey_id == NVKEY_DSP_PARA_WIND_DETECT) {
        stream_function_wind_load_nvkey(share_ptr);
#endif
#ifdef AIR_ANC_USER_UNAWARE_ENABLE
    } else if (nvkey_id == NVKEY_DSP_PARA_USR_UNAWARE) {
        stream_function_user_unaware_load_nvkey(share_ptr);
#endif
#ifdef MTK_USER_TRIGGER_FF_ENABLE
    } else if (nvkey_id == NVKEY_DSP_PARA_ADAPTIVE_FF) {
        stream_function_user_trigger_ff_load_nvkey(share_ptr);
#endif
#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
    } else if (nvkey_id == NVKEY_DSP_PARA_ENVIRONMENT_DETECTION) {
        stream_function_environment_detection_load_nvkey(share_ptr);
#endif
#ifdef AIR_SILENCE_DETECTION_ENABLE
    } else if (nvkey_id == NVKEY_DSP_PARA_SILENCE_DETECTION){
        Sink_Audio_SilenceDetection_Load_Nvkey(share_ptr);
#endif
#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
    } else if ((nvkey_id >= NVKEY_DSP_PARA_AFC) && (nvkey_id <= NVKEY_DSP_PARA_ADVANCED_PASSTHROUGH_MISC)){
        advanced_passthrough_hearing_aid_features_set_param(nvkey_id, share_ptr);
#endif
    } else {
        DSP_MW_LOG_E("DSP set algorithm parameters nvkey id is unsupported 0x%x", 1, nvkey_id);
    }
    UNUSED(ack);
}

#ifdef AIR_ANC_USER_UNAWARE_ENABLE
audio_extend_gain_control_t g_audio_extend_gain_ctrl[AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_NUM];
#endif
void dsp_anc_monitor_extend_gain_ctrl_init(void)
{
    #ifdef AIR_ANC_USER_UNAWARE_ENABLE
    memset(g_audio_extend_gain_ctrl, 0, sizeof(g_audio_extend_gain_ctrl));
    #endif
}

void dsp_anc_monitor_extend_gain_ctrl_set(audio_extend_gain_control_ptr_t gain_control)
{
    #ifdef AIR_ANC_USER_UNAWARE_ENABLE
    if ((gain_control->gain_type < AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_NUM)
            && (gain_control->gain_type != AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_USER_UNAWARE)) {
        memcpy(&g_audio_extend_gain_ctrl[gain_control->gain_type], gain_control, sizeof(audio_extend_gain_control_t));
    }
    #else
    UNUSED(gain_control);
    #endif
}

void dsp_anc_monitor_extend_gain_ctrl_get_total(int16_t *get_total_gain0, int16_t *get_total_gain1)
{
    #ifdef AIR_ANC_USER_UNAWARE_ENABLE
    int16_t total_gain[2] = {0,0};
    for (int i=0; i<AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_NUM; i++) {
        if (i == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_USER_UNAWARE)
            continue;
        total_gain[0] += g_audio_extend_gain_ctrl[i].gain[0];
        total_gain[1] += g_audio_extend_gain_ctrl[i].gain[1];
    }
    *get_total_gain0 = total_gain[0];
    *get_total_gain1 = total_gain[1];
    #else
    UNUSED(get_total_gain0);
    UNUSED(get_total_gain1);
    #endif
}

void dsp_ccni_set_extend_gain(audio_extend_gain_control_ptr_t gain_control)
{
    hal_ccni_message_t ccni_msg;
    memset((void *)&ccni_msg, 0, sizeof(hal_ccni_message_t));
    ccni_msg.ccni_message[0] = ((MSG_DSP2MCU_COMMON_AUDIO_ANC_ADAPTIVE << 16) |
                                (gain_control->gain_type&0xFF) |
                                (((U16)gain_control->misc)<<8));
    ccni_msg.ccni_message[1] =  (((gain_control->gain[1]&0xFFFF)<<16)|
                                 (gain_control->gain[0]&0xFFFF));
    aud_msg_tx_handler(ccni_msg, 0, FALSE);

    dsp_anc_monitor_extend_gain_ctrl_set(gain_control);
}

void dsp_set_adaptive_anc(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    audio_anc_monitor_set_info_t set_type;
    uint32_t data;
    set_type = (audio_anc_monitor_set_info_t)(msg.ccni_message[0]&0xFFFF);
    data = msg.ccni_message[1];
    UNUSED(ack);
    if (0) {
#ifdef AIR_ANC_USER_UNAWARE_ENABLE
    } else if (set_type < AUDIO_ANC_MONITOR_SET_USER_UNAWARE_MAX) {
        stream_function_user_unaware_inform_handler(set_type, data);
#endif
#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
    } else if (set_type < AUDIO_ANC_MONITOR_SET_ENVIRONMENT_DETECTION_MAX) {
        stream_function_environment_detection_set_value(set_type, data);
#endif
    } else {
        DSP_MW_LOG_I("DSP set adaptive anc, not support type:0x%x", 1, set_type);
    }
}

void dsp_get_adaptive_anc(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    audio_anc_monitor_get_info_t get_type;
    uint32_t *data_addr;

    get_type = (audio_anc_monitor_get_info_t)(msg.ccni_message[0]&0xFFFF);
    data_addr = (uint32_t *)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    if (0) {
#ifdef AIR_ANC_USER_UNAWARE_ENABLE
    } else if (get_type == AUDIO_ANC_MONITOR_GET_USER_UNAWARE_GAIN_INFO) {
        stream_function_user_unaware_get_info((int32_t *)data_addr);
    } else if (get_type == AUDIO_ANC_MONITOR_GET_USER_UNAWARE_ENABLE_STAT) {
        *data_addr = (uint32_t)stream_function_user_unaware_get_enable_state();
#endif
#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
    } else if (get_type == AUDIO_ANC_MONITOR_GET_ENVIRONMENT_DETECTION_STATIONARY_NOISE) {
        *data_addr = (uint32_t)stream_function_environment_detection_get_value(get_type);
#endif
    } else {
        DSP_MW_LOG_I("DSP get adaptive anc, not support type:0x%x", 1, get_type);
    }
}


#ifdef AIR_ANC_USER_UNAWARE_ENABLE
const uint32_t gUser_Unaware_en = 0x00270027;
void dsp_get_user_unaware_info(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    uint16_t type = (uint16_t)(msg.ccni_message[0]&0xFFFF);
    S32* UU_info_start_addr;
    UU_info_start_addr = (S32*)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    if (type == 0) {
        stream_function_user_unaware_get_info(UU_info_start_addr);
    } else if (type == 1){
        *UU_info_start_addr = (U32)stream_function_user_unaware_get_enable_state();
    }
}
#else
const uint32_t gUser_Unaware_en = 0;

#endif

#ifdef MTK_PROMPT_SOUND_ENABLE
uint32_t gDSP_VP_GPT_TIMER_HANDLE = 0;
uint32_t gDSP_VP_POLLING_TIME = 600; //us
uint32_t gDSP_VP_GPT_TARGET_TIME = 0;
void CB_CM4_VP_PLAYBACK_GPT_CALLBACK(void *user_data)
{
    UNUSED(user_data);
    uint32_t savedmask;
    uint32_t curr_cnt = 0;
    hal_nvic_save_and_set_interrupt_mask(&savedmask);
    //hal_gpio_set_output(HAL_GPIO_2, 0);
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
    //DSP_MW_LOG_I("[VPC][SYNC]Enter VP DSP polling!!, curr_cnt = %d.\n", 1, curr_cnt);
    if(gDSP_VP_GPT_TARGET_TIME > curr_cnt){
        while(1){
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
            //DSP_MW_LOG_I("[VPC][SYNC]polling!!, curr_cnt = %d, Target_cnt = %d.\n", 2, curr_cnt, gDSP_VP_GPT_TARGET_TIME);
            if(curr_cnt >= gDSP_VP_GPT_TARGET_TIME){
                break;
            }
        }
        *((volatile uint32_t*)(0xC0000010)) |= 0x00000004; //Patch test
        //hal_gpio_set_output(HAL_GPIO_2, 1);
    #ifndef HAL_AUDIO_ENABLE_PATH_MEM_DEVICE
        audio_digital_block_t memory_block;
        memory_block = hal_audio_afe_get_memory_digital_block (playback_vp_if.sink->param.audio.memory, true);
        afe_enable_audio_irq(afe_irq_request_number(memory_block), playback_vp_if.sink->param.audio.rate, playback_vp_if.sink->param.audio.count);
        afe_set_memory_path_enable(memory_block, !(playback_vp_if.sink->param.audio.AfeBlkControl.u4awsflag), true);
    #else
        hal_audio_trigger_start_parameter_t sw_trigger_start;
        sw_trigger_start.enable = true;
        sw_trigger_start.memory_select = playback_vp_if.sink->param.audio.mem_handle.memory_select;
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&sw_trigger_start,HAL_AUDIO_SET_TRIGGER_MEMORY_START);
    #endif
        //hal_gpio_set_output(HAL_GPIO_2, 0);
    }else{
        if((curr_cnt - gDSP_VP_GPT_TARGET_TIME) > 0x80000000){
            //gDSP_VP_GPT_TARGET_TIME overflow
            while(1){
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
                //DSP_MW_LOG_I("[VPC][SYNC]polling!!, curr_cnt = %d, Target_cnt = %d.\n", 2, curr_cnt, gDSP_VP_GPT_TARGET_TIME);
                if(curr_cnt >= gDSP_VP_GPT_TARGET_TIME){
                    if((curr_cnt & 0x80000000) == 0x0){
                        break;
                    }
                }
            }
            *((volatile uint32_t*)(0xC0000010)) |= 0x00000004; //Patch test
            //hal_gpio_set_output(HAL_GPIO_2, 1);
        #ifndef HAL_AUDIO_ENABLE_PATH_MEM_DEVICE
            audio_digital_block_t memory_block;
            memory_block = hal_audio_afe_get_memory_digital_block (playback_vp_if.sink->param.audio.memory, true);
            afe_enable_audio_irq(afe_irq_request_number(memory_block), playback_vp_if.sink->param.audio.rate, playback_vp_if.sink->param.audio.count);
            afe_set_memory_path_enable(memory_block, !(playback_vp_if.sink->param.audio.AfeBlkControl.u4awsflag), true);
        #else
            hal_audio_trigger_start_parameter_t sw_trigger_start;
            sw_trigger_start.enable = true;
            sw_trigger_start.memory_select = playback_vp_if.sink->param.audio.mem_handle.memory_select;
            hal_audio_set_value((hal_audio_set_value_parameter_t *)&sw_trigger_start,HAL_AUDIO_SET_TRIGGER_MEMORY_START);
        #endif
            //hal_gpio_set_output(HAL_GPIO_2, 0);
        }else{
            //hal_gpio_set_output(HAL_GPIO_2, 1);
            *((volatile uint32_t*)(0xC0000010)) |= 0x00000004;
        #ifndef HAL_AUDIO_ENABLE_PATH_MEM_DEVICE
            audio_digital_block_t memory_block;
            memory_block = hal_audio_afe_get_memory_digital_block (playback_vp_if.sink->param.audio.memory, true);
            afe_enable_audio_irq(afe_irq_request_number(memory_block), playback_vp_if.sink->param.audio.rate, playback_vp_if.sink->param.audio.count);
            afe_set_memory_path_enable(memory_block, !(playback_vp_if.sink->param.audio.AfeBlkControl.u4awsflag), true);
        #else
            hal_audio_trigger_start_parameter_t sw_trigger_start;
            sw_trigger_start.enable = true;
            sw_trigger_start.memory_select = playback_vp_if.sink->param.audio.mem_handle.memory_select;
            hal_audio_set_value((hal_audio_set_value_parameter_t *)&sw_trigger_start,HAL_AUDIO_SET_TRIGGER_MEMORY_START);
        #endif
            //hal_gpio_set_output(HAL_GPIO_2, 0);
            //hal_nvic_restore_interrupt_mask(savedmask);
            DSP_MW_LOG_E("[CM4_VP_PB][ERROR] polling callback t(%d) < c(%d) cnt error", 2, gDSP_VP_GPT_TARGET_TIME, curr_cnt);
            //platform_assert("vp sync polling callback target < current GPT count",__FILE__,__LINE__);
        }
    }
    hal_nvic_restore_interrupt_mask(savedmask);

    hal_gpt_status_t gpt_status = hal_gpt_sw_free_timer(gDSP_VP_GPT_TIMER_HANDLE);
    if(HAL_GPT_STATUS_OK != gpt_status){
        DSP_MW_LOG_I("[VPC][SYNC]VP free one_shot Timer error!!, error id = %d.\n", 1, gpt_status);
    }
    DSP_MW_LOG_I("[VPC][SYNC]Polling end!!, curr_cnt = %d,target_cnt = %d.\n", 2, curr_cnt, gDSP_VP_GPT_TARGET_TIME);
    gDSP_VP_GPT_TIMER_HANDLE = 0;
    gDSP_VP_GPT_TARGET_TIME = 0;
    dsp_audio_msg_ack(MSG_MCU2DSP_VP_SYNC_REQUEST<<8|SUBMSG_MCU2DSP_SYNC_START, true); // replace with volume sync mechanism
}

/* CM4 VP Playback CCNI callback function */
volatile uint32_t vp_config_flag = 0;
void CB_CM4_VP_PLAYBACK_OPEN(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("[CM4_VP_PB] Open", 0);
    vp_config_flag = 0;

    /* remap to non-cacheable address */
    mcu2dsp_open_param_p open_param;
    open_param = (mcu2dsp_open_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);

#ifdef ENABLE_HWSRC_CLKSKEW
#ifdef AIR_HWSRC_TX_TRACKING_ENABLE
    open_param->stream_out_param.afe.clkskew_mode = CLK_SKEW_V1;
#endif
#endif

    DSP_REMAP_SHARE_INFO(open_param->stream_in_param.playback.share_info_base_addr, uint32_t);

    playback_vp_if.source = dsp_open_stream_in(open_param);//StreamCM4VPPlaybackSource(share_info);
    playback_vp_if.sink   = dsp_open_stream_out(open_param);//StreamAudioAfe2Sink(AUDIO_HARDWARE_PCM, INSTANCE_A, AUDIO_CHANNEL_A_AND_B);
    playback_vp_if.transform = NULL;
    //stream_feature_configure_type(stream_feature_list_prompt, CODEC_PCM_COPY, CONFIG_DECODER);
}

void CB_CM4_VP_PLAYBACK_START(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("[CM4_VP_PB] Start", 0);
    bool time_out_flag  = 0;
    uint32_t curr_cnt   = 0;
    uint32_t target_cnt = 0;
    uint32_t time_out   = 0;
    uint32_t savedmask;
    hal_gpt_status_t gpt_status;
    mcu2dsp_start_param_p start_param;
    start_param = (mcu2dsp_start_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);

    dsp_start_stream_in (start_param, playback_vp_if.source);
    dsp_start_stream_out(start_param, playback_vp_if.sink);

    playback_vp_if.transform = TrasformAudio2Audio(playback_vp_if.source, playback_vp_if.sink, stream_feature_list_prompt);
    if (playback_vp_if.transform == NULL)
    {
        DSP_MW_LOG_E("[CM4_VP_PB] transform failed", 0);
    }
    hal_nvic_save_and_set_interrupt_mask(&savedmask);
    if(playback_vp_if.sink->param.audio.aws_sync_request){
        //hal_gpio_set_output(HAL_GPIO_2, 1);
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
        target_cnt = playback_vp_if.sink->param.audio.aws_sync_time;
        if(target_cnt < curr_cnt){
            if((curr_cnt - target_cnt) > 0x80000000){
                //target_cnt overflow
                time_out = (0xFFFFFFFF - curr_cnt) + target_cnt - gDSP_VP_POLLING_TIME;
            } else {
                DSP_MW_LOG_E("[CM4_VP_PB][ERROR] t(%d) < c(%d) cnt error", 2, target_cnt, curr_cnt);
                //platform_assert("vp sync target < current GPT count",__FILE__,__LINE__);
                time_out_flag = true;
            }
        }else if((target_cnt - curr_cnt) < 600){
            DSP_MW_LOG_E("[CM4_VP_PB][ERROR] t(%d) - c(%d) < 600us cnt error", 2, target_cnt, curr_cnt);
            //platform_assert("vp sync (target-current) < gDSP_VP_POLLING_TIME",__FILE__,__LINE__);
            time_out_flag = true;
        }else{
            time_out =(target_cnt - curr_cnt) - gDSP_VP_POLLING_TIME;
        }
        if(!time_out_flag){
            gDSP_VP_GPT_TARGET_TIME = target_cnt;
            if(gDSP_VP_GPT_TIMER_HANDLE == 0){
                gpt_status = hal_gpt_sw_get_timer(&gDSP_VP_GPT_TIMER_HANDLE);
                if(HAL_GPT_STATUS_OK != gpt_status){
                    DSP_MW_LOG_I("[VPC][SYNC]VP get one_shot Timer error!!, error id = %d.\n", 1, gpt_status);
                }
            }
            //hal_gpio_set_output(HAL_GPIO_2, 0);
            gpt_status = hal_gpt_sw_start_timer_us(gDSP_VP_GPT_TIMER_HANDLE, time_out, CB_CM4_VP_PLAYBACK_GPT_CALLBACK, NULL);
            //hal_gpio_set_output(HAL_GPIO_2, 1);
            if(HAL_GPT_STATUS_OK != gpt_status){
                DSP_MW_LOG_I("[VPC][SYNC]VP start one_shot Timer error!!, error id = %d.\n", 1, gpt_status);
            }
        } else {
            if(afe.dl2_enable){
                *((volatile uint32_t*)(0xC0000010)) |= 0x00000004;
            #ifndef HAL_AUDIO_ENABLE_PATH_MEM_DEVICE
                audio_digital_block_t memory_block;
                memory_block = hal_audio_afe_get_memory_digital_block (playback_vp_if.sink->param.audio.memory, true);
                afe_enable_audio_irq(afe_irq_request_number(memory_block), playback_vp_if.sink->param.audio.rate, playback_vp_if.sink->param.audio.count);
                afe_set_memory_path_enable(memory_block, !(playback_vp_if.sink->param.audio.AfeBlkControl.u4awsflag), true);
            #else
                hal_audio_trigger_start_parameter_t sw_trigger_start;
                sw_trigger_start.enable = true;
                sw_trigger_start.memory_select = playback_vp_if.sink->param.audio.mem_handle.memory_select;
                hal_audio_set_value((hal_audio_set_value_parameter_t *)&sw_trigger_start,HAL_AUDIO_SET_TRIGGER_MEMORY_START);
            #endif
            } else {
                DSP_MW_LOG_I("[CM4_VP_PB] VP not exit or ready.", 0);
            }
        }
    }
    hal_nvic_restore_interrupt_mask(savedmask);
    if(time_out_flag){
        xTaskResumeFromISR((TaskHandle_t)pDPR_TaskHandler);
        portYIELD_FROM_ISR(pdTRUE); // force to do context switch
    }
    #ifdef AIR_SILENCE_DETECTION_ENABLE
    Sink_Audio_SilenceDetection_Register(playback_vp_if.sink);
    #endif
    DSP_MW_LOG_I("[CM4_VP_PB] Start info request(%d), c_cnt(%d), t_cnt(%d), time_out(%d)", 4, playback_vp_if.sink->param.audio.aws_sync_request, curr_cnt, target_cnt, time_out);
}


void CB_CM4_VP_PLAYBACK_STOP(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    uint32_t savedmask;
    #ifdef AIR_SILENCE_DETECTION_ENABLE
    Sink_Audio_SilenceDetection_Unregister(playback_vp_if.sink);
    #endif
    hal_nvic_save_and_set_interrupt_mask(&savedmask);
    if(gDSP_VP_GPT_TIMER_HANDLE != 0){
        hal_gpt_status_t gpt_status = hal_gpt_sw_stop_timer_us(gDSP_VP_GPT_TIMER_HANDLE);
        if(HAL_GPT_STATUS_OK != gpt_status){
            DSP_MW_LOG_I("[VPC][SYNC]VP stop one_shot Timer error!!, error id = %d.\n", 1, gpt_status);
        }
        gpt_status = hal_gpt_sw_free_timer(gDSP_VP_GPT_TIMER_HANDLE);
        if(HAL_GPT_STATUS_OK != gpt_status){
            DSP_MW_LOG_I("[VPC][SYNC]VP free2 one_shot Timer error!!, error id = %d.\n", 1, gpt_status);
        }
        gDSP_VP_GPT_TIMER_HANDLE = 0;
    }
    hal_nvic_restore_interrupt_mask(savedmask);
    DSP_MW_LOG_I("[CM4_VP_PB] Stop", 0);
    if (playback_vp_if.transform != NULL)
    {
        StreamDSPClose(playback_vp_if.transform->source, playback_vp_if.transform->sink, msg.ccni_message[0]>>16|0x8000);
    }
    playback_vp_if.transform = NULL;
}

void CB_CM4_VP_PLAYBACK_CONFIG(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    uint16_t vp_stop_immediately = hal_memview_cm4_to_dsp0(msg.ccni_message[0]);
    UNUSED(ack);
    /*
      Todo:
      Notify DSP framework to push remained pcm data into AFE.
    */

    /* Set flag to notify APP to stop VP */
    if (playback_vp_if.transform != NULL){
      #if 0 //VP log slim
        DSP_MW_LOG_I("[CM4_VP_PB] Config", 0);
      #endif
        vp_config_flag = 1;
        if (vp_stop_immediately == 0x55) {
            // clear audio buffer
            DSP_CALLBACK_PTR callback_ptr = DSP_Callback_Get(playback_vp_if.source, playback_vp_if.sink);
            memset(callback_ptr->EntryPara.in_ptr[0], 0, callback_ptr->EntryPara.in_malloc_size);
            memset(callback_ptr->EntryPara.out_ptr[0], 0, callback_ptr->EntryPara.out_malloc_size);
            // Clear SRC IN
            memset((uint32_t *)(playback_vp_if.sink->param.audio.AfeBlkControl.phys_buffer_addr + playback_vp_if.sink->param.audio.AfeBlkControl.u4asrc_buffer_size), 0, AFE_INTERNAL_SRAM_VP_SIZE-playback_vp_if.sink->param.audio.AfeBlkControl.u4asrc_buffer_size);
            // Clear SRC OUT
            memset((uint32_t *)playback_vp_if.sink->param.audio.AfeBlkControl.phys_buffer_addr, 0, playback_vp_if.sink->param.audio.AfeBlkControl.u4asrc_buffer_size);
            DSP_MW_LOG_I("[CM4_VP_PB] config stop immediately", 0);
        }
    } else {
        DSP_MW_LOG_I("[CM4_VP_PB] Config duplicate.", 0);
    }
}

void CB_CM4_VP_PLAYBACK_CLOSE(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("[CM4_VP_PB] Close", 0);
    SourceClose(playback_vp_if.source);
    SinkClose(playback_vp_if.sink);
    DSP_PIC_FeatureDeinit(playback_vp_if.pfeature_table);
    memset(&playback_vp_if,0,sizeof(CONNECTION_IF));
}

void CB_CM4_VP_PLAYBACK_TRIGGER(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("[CM4_VP_PB] Trigger Just return", 0);
    return;
  #if 0
    if(afe.dl2_enable){
        //DSP_MW_LOG_I("[CM4_VP_PB] VP trigger memory irq enable", 0);
        #ifndef HAL_AUDIO_ENABLE_PATH_MEM_DEVICE
        audio_digital_block_t memory_block;
        memory_block = hal_audio_afe_get_memory_digital_block (playback_vp_if.sink->param.audio.memory, true);
        afe_enable_audio_irq(afe_irq_request_number(memory_block), playback_vp_if.sink->param.audio.rate, playback_vp_if.sink->param.audio.count);
        afe_set_memory_path_enable(memory_block, !(playback_vp_if.sink->param.audio.AfeBlkControl.u4awsflag), true);
        #else
        hal_audio_trigger_start_parameter_t sw_trigger_start;
        sw_trigger_start.enable = true;
        sw_trigger_start.memory_select = playback_vp_if.sink->param.audio.mem_handle.memory_select;
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&sw_trigger_start,HAL_AUDIO_SET_TRIGGER_MEMORY_START);
        #endif
        xTaskResumeFromISR((TaskHandle_t)pDPR_TaskHandler);
        portYIELD_FROM_ISR(pdTRUE); // force to do context switch
    }else {
        DSP_MW_LOG_I("[CM4_VP_PB] VP not exit or ready.", 0);
    }
  #endif
}
#endif

#ifdef AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
/* VP_DUMMY Playback CCNI callback function */
extern SHARE_BUFFER_INFO DUMMY_buff_info;
void CB_CM4_VP_DUMMY_SOURCE_PLAYBACK_OPEN(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("[CM4_VP_DUMMY] Open", 0);

    /* remap to non-cacheable address */
    mcu2dsp_open_param_p open_param;
    open_param = (mcu2dsp_open_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    //DSP_REMAP_SHARE_INFO(open_param->stream_in_param.playback.share_info_base_addr);
    open_param->stream_in_param.playback.share_info_base_addr = &DUMMY_buff_info;
    playback_vp_dummy_source_if.source = dsp_open_stream_in(open_param);
    playback_vp_dummy_source_if.sink   = dsp_open_stream_out(open_param);
    playback_vp_dummy_source_if.transform = NULL;
}

void CB_CM4_VP_DUMMY_SOURCE_PLAYBACK_START(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("[CM4_VP_DUMMY] Start", 0);
    mcu2dsp_start_param_p start_param;
    start_param = (mcu2dsp_start_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);

    dsp_start_stream_in (start_param, playback_vp_dummy_source_if.source);
    dsp_start_stream_out(start_param, playback_vp_dummy_source_if.sink);
    //Rdebug- Needcheck
    //DSP_ConfigFeatureListCodecResolution(playback_vp_dummy_source_if.pfeature_table, Res_32bit, CONFIG_DECODER); /*Set Feature Codec 32 Resolution.*/

    playback_vp_dummy_source_if.transform = TrasformAudio2Audio(playback_vp_dummy_source_if.source, playback_vp_dummy_source_if.sink, stream_feature_list_prompt_dummy_source);
    if (playback_vp_dummy_source_if.transform == NULL)
    {
        DSP_MW_LOG_I("[CM4_VP_DUMMY] transform failed", 0);
    }
}


void CB_CM4_VP_DUMMY_SOURCE_PLAYBACK_STOP(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("[CM4_VP_DUMMY] Stop", 0);
    uint32_t delay_20ms = pdMS_TO_TICKS(20);
    vTaskDelay(delay_20ms); // reduce closing noise, 2 tick = 20ms
    if (playback_vp_dummy_source_if.transform != NULL)
    {
        StreamDSPClose(playback_vp_dummy_source_if.transform->source, playback_vp_dummy_source_if.transform->sink, msg.ccni_message[0]>>16|0x8000);
    }
    DSP_CALLBACK_PTR callback_ptr = DSP_Callback_Get(playback_vp_dummy_source_if.source, playback_vp_dummy_source_if.sink);
    if (callback_ptr) {
        memset(callback_ptr->EntryPara.in_ptr[0], 0, callback_ptr->EntryPara.in_malloc_size);
        memset(callback_ptr->EntryPara.out_ptr[0], 0, callback_ptr->EntryPara.out_malloc_size);
    }
    playback_vp_dummy_source_if.transform = NULL;
}

void CB_CM4_VP_DUMMY_SOURCE_PLAYBACK_CLOSE(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);
    DSP_MW_LOG_I("[CM4_VP_DUMMY] Close", 0);
    SourceClose(playback_vp_dummy_source_if.source);
    SinkClose(playback_vp_dummy_source_if.sink);
    DSP_PIC_FeatureDeinit(playback_vp_dummy_source_if.pfeature_table);
    memset(&playback_vp_dummy_source_if,0,sizeof(CONNECTION_IF));
}

extern void cm4_vp_dummy_source_playback_set_param(uint8_t mode, uint8_t index);
void CB_CM4_VP_DUMMY_SOURCE_PLAYBACK_CHANGE_FEATURE(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    DSP_MW_LOG_I("[CM4_VP_DUMMY] Change feature", 0);
    uint8_t mode = (uint8_t)msg.ccni_message[1];
    uint8_t index = (uint8_t)(msg.ccni_message[1] >> 16);
    cm4_vp_dummy_source_playback_set_param(mode, index);
}
#endif

#ifdef AIR_BT_CLK_SKEW_ENABLE
void CB_N9_CLK_SKEW_LAG(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    if (msg.ccni_message[1] == ClkSkewLagsNo) {
        #ifdef DSP_CLK_SKEW_DEBUG_LOG
        DSP_MW_LOG_I("[CLK_SKEW] Lags", 0);
        #endif
        clk_skew_inform_dl_lags_samples(1);
        clk_skew_inform_ul_lags_samples(1);

        ClkSkewLagsNo++;
    }
    else {
        #ifdef DSP_CLK_SKEW_DEBUG_LOG
        DSP_MW_LOG_I("[CLK_SKEW] Lags not from N9, msg:%d", 1, msg.ccni_message[1]);
        #endif
    }

}

void CB_N9_CLK_SKEW_LEAD(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    if (msg.ccni_message[1] == ClkSkewLeadsNo) {
        #ifdef DSP_CLK_SKEW_DEBUG_LOG
        DSP_MW_LOG_I("[CLK_SKEW] Leads", 0);
        #endif
        clk_skew_inform_dl_leads_samples(1);
        clk_skew_inform_ul_leads_samples(1);

        ClkSkewLeadsNo++;
    }
    else {
        #ifdef DSP_CLK_SKEW_DEBUG_LOG
        DSP_MW_LOG_I("[CLK_SKEW] Leads not from N9, msg:%d", 1, msg.ccni_message[1]);
        #endif
    }

}
#endif

void CB_CM4_AUDIO_AMP_FORCE_CLOSE(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    UNUSED(msg);
    DSP_MW_LOG_I("[CM4_AUDIO] AMP Force Close", 0);
#ifdef ENABLE_AMP_TIMER
    fw_amp_force_close();
#endif
}

void dsp_set_audio_device_parameters(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
#ifdef AIR_AUDIO_HARDWARE_ENABLE
    audio_driver_set_info_t set_type;
    uint32_t data;
    set_type = (audio_anc_monitor_set_info_t)(msg.ccni_message[0]&0xFFFF);
    data = msg.ccni_message[1];
    UNUSED(ack);
    if (0) {
    } else if (set_type == AUDIO_DRIVER_SET_HOLD_AMP_GPIO) {
        afe_set_hold_amp_gpio((uint32_t)data);
    } else {
        DSP_MW_LOG_I("DSP set audio device parameters, not support type:0x%x", 1, set_type);
    }
#else
    UNUSED(msg);
    UNUSED(ack);
#endif /* AIR_AUDIO_HARDWARE_ENABLE */
}

#ifdef MTK_SENSOR_SOURCE_ENABLE
#include "bsp_multi_axis_sensor.h"
extern bool bsp_multi_axis_read_register(uint32_t addr, uint8_t *data);
extern bool bsp_multi_axis_write_register(uint32_t addr, uint8_t *data);

void CB_GSENSOR_DETECT_READ_RG(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    uint8_t data = 0;

    ack = ack;

    if (bsp_multi_axis_read_register((uint32_t)msg.ccni_message[0]&0xffff,&data) == false) {
        log_hal_msgid_error("[G-sensor] CB_CM4_GSENSOR_READ_RG fail",0);
        return;
    }
    log_hal_msgid_info("[G-sensor]CB_CM4_GSENSOR_READ_RG,addr = 0x%08x,data = 0x%08x",2,msg.ccni_message[0]&0xffff,data);
}

void CB_GSENSOR_DETECT_WRITE_RG(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    uint8_t data = 0;

    ack = ack;

    data = (uint8_t)msg.ccni_message[1];
    if (bsp_multi_axis_write_register((uint32_t)msg.ccni_message[0]&0xffff,&data) == false) {
        log_hal_msgid_error("[G-sensor] CB_CM4_GSENSOR_WRITE_RG fail",0);
        return;
    }

    log_hal_msgid_info("[G-sensor]CB_CM4_GSENSOR_WRITE_RG,addr = 0x%08x,data = 0x%08x",2,msg.ccni_message[0]&0xffff,data);
}
#endif
#ifdef MTK_AUDIO_PLC_ENABLE
void DSP_AUDIO_PLC_CONTROL(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("DSP_AUDIO_PLC_CONTROL\r\n", 0);

    uint32_t enable = (uint32_t)(msg.ccni_message[1]);
    dsp_audio_plc_ctrl_t audio_plc_ctrl;
    audio_plc_ctrl.enable = enable;
    Audio_PLC_ctrl(audio_plc_ctrl);
    DSP_MW_LOG_I("DSP_AUDIO_PLC_CONTROL ENABLE %d\r\n", 1,enable);
}
#endif

#ifdef AIR_BT_CLK_SKEW_ENABLE
extern void clk_skew_enable_debug_log(bool enable);
void DSP_CLK_SKEW_DEBUG_CONTROL(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    uint32_t enable = (uint32_t)(msg.ccni_message[1]);
    clk_skew_enable_debug_log((bool)enable);
    DSP_MW_LOG_I("DSP_CLK_SKEW_DEBUG_CONTROL ENABLE %d\r\n", 1,enable);
}
#endif

#ifdef MTK_DSP_SHUTDOWN_SPECIAL_CONTROL_ENABLE
void DSP_DUMMY_SHUTDOWN(void)
{
    bool ack = false;
    uint32_t savedmask;
    uint16_t count = 1;
    hal_nvic_save_and_set_interrupt_mask(&savedmask);
    //**Special control.
    //**To avoid DSP task HW semaphore when CM4 disable DSP power by using SPM control.
    hal_ccni_message_t msg;
    memset((void *)&msg, 0, sizeof(hal_ccni_message_t));
    msg.ccni_message[0] = 0x804b << 16;
    while(1){
        count ++;
        if((count % 1000) == 0){
            if(!ack){
                if(hal_ccni_set_event(AUDIO_CM4_TX_EVENT, &msg) == HAL_CCNI_STATUS_OK){ /*Remove all OS API, just call driver.*/
                    ack = true;
                }
            }
        }
    }
}
#endif

#ifdef MTK_AUDIO_TRANSMITTER_ENABLE

#ifdef MTK_MULTI_MIC_STREAM_ENABLE
extern stream_feature_list_t stream_featuremulti_mic_function_a[];
extern stream_feature_list_t stream_featuremulti_mic_function_b[];
extern stream_feature_list_t stream_featuremulti_mic_function_c[];
extern stream_feature_list_t stream_featuremulti_mic_function_f[];

CONNECTION_IF g_multi_mic_streams[AUDIO_TRANSMITTER_MULTI_MIC_STREAM_SUB_ID_MAX] = {
    /* source     sink            transform     pfeature_table */
    {NULL,      NULL,       NULL,        stream_featuremulti_mic_function_a},  //AUDIO_SCENARIO_MULTI_MIC_STREAM_FUNCTION_A
    {NULL,      NULL,       NULL,        stream_featuremulti_mic_function_b},  //AUDIO_SCENARIO_MULTI_MIC_STREAM_FUNCTION_B
    {NULL,      NULL,       NULL,        stream_featuremulti_mic_function_c},  //AUDIO_SCENARIO_MULTI_MIC_STREAM_FUNCTION_C
    {NULL,      NULL,       NULL,        stream_featuremulti_mic_function_f},  //AUDIO_SCENARIO_MULTI_MIC_STREAM_FUNCTION_F
};
#endif

#ifdef MTK_SENSOR_SOURCE_ENABLE
CONNECTION_IF g_sensor_streams = {
    /* source     sink            transform     pfeature_table */
    NULL,        NULL,       NULL,       AudioFeatureList_GSensorMotionDetect
};
#endif

#ifdef MTK_AUDIO_LOOPBACK_TEST_ENABLE
extern stream_feature_list_t stream_feature_list_audio_loopback_test[];
CONNECTION_IF audio_loopback_test_if = {NULL,      NULL,       NULL,     stream_feature_list_audio_loopback_test};
#endif

#if defined(MTK_GAMING_MODE_HEADSET) || defined(AIR_GAMING_MODE_DONGLE_ENABLE)
extern stream_feature_list_t stream_feature_list_usb_in_broadcast_0[];
CONNECTION_IF g_usb_in_broadcast_streams_0 = {
    /* source     sink            transform     pfeature_table */
    NULL,        NULL,       NULL,       stream_feature_list_usb_in_broadcast_0
};

extern stream_feature_list_t stream_feature_list_usb_in_broadcast_1[];
CONNECTION_IF g_usb_in_broadcast_streams_1 = {
    /* source     sink            transform     pfeature_table */
    NULL,        NULL,       NULL,       stream_feature_list_usb_in_broadcast_1
};

extern stream_feature_list_t stream_feature_list_usb_out_broadcast[];
CONNECTION_IF g_usb_out_broadcast_streams = {
    /* source     sink            transform     pfeature_table */
    NULL,        NULL,       NULL,       stream_feature_list_usb_out_broadcast
};
#endif /* MTK_GAMING_MODE_HEADSET || AIR_GAMING_MODE_DONGLE_ENABLE */

#if defined(MTK_GAMING_MODE_HEADSET) || defined(AIR_GAMING_MODE_DONGLE_ENABLE)
extern stream_feature_list_t stream_feature_list_game_headset_ul[];
CONNECTION_IF g_game_headset_ul_streams = {
    /* source     sink            transform     pfeature_table */
    NULL,        NULL,       NULL,       stream_feature_list_game_headset_ul
};
#endif /* MTK_GAMING_MODE_HEADSET || AIR_GAMING_MODE_DONGLE_ENABLE */

#ifdef MTK_TDM_ENABLE
extern stream_feature_list_t stream_feature_list_tdm[];
CONNECTION_IF tdm_if = {NULL,      NULL,       NULL,     stream_feature_list_tdm};
#endif

#if defined(AIR_WIRED_AUDIO_ENABLE)
extern stream_feature_list_t stream_feature_list_usb_in_local_0[];
CONNECTION_IF g_usb_in_local_streams_0 = {
    /* source     sink            transform     pfeature_table */
    NULL,        NULL,       NULL,       stream_feature_list_usb_in_local_0
};
extern stream_feature_list_t stream_feature_list_usb_in_local_1[];
CONNECTION_IF g_usb_in_local_streams_1 = {
    /* source     sink            transform     pfeature_table */
    NULL,        NULL,       NULL,       stream_feature_list_usb_in_local_1
};
extern stream_feature_list_t stream_feature_list_line_in_local[];
CONNECTION_IF g_line_in_local_streams = {
    /* source     sink            transform     pfeature_table */
    NULL,        NULL,       NULL,       stream_feature_list_line_in_local
};
extern stream_feature_list_t stream_feature_list_usb_out_local[];
CONNECTION_IF g_usb_out_local_streams = {
    /* source     sink            transform     pfeature_table */
    NULL,        NULL,       NULL,       stream_feature_list_usb_out_local
};
extern stream_feature_list_t stream_feature_list_line_out_local[];
CONNECTION_IF g_line_out_local_streams = {
    /* source     sink            transform     pfeature_table */
    NULL,        NULL,       NULL,       stream_feature_list_line_out_local
};


#endif /*AIR_WIRED_AUDIO_ENABLE*/

#if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
extern stream_feature_list_t stream_feature_list_advanced_passthrough[];
CONNECTION_IF advanced_passthrough_if = {NULL,      NULL,       NULL,     stream_feature_list_advanced_passthrough};
#endif /* AIR_ADVANCED_PASSTHROUGH_ENABLE */

#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
extern stream_feature_list_t stream_feature_list_ble_audio_dongle_usb_in_broadcast_0[];
CONNECTION_IF g_ble_audio_dongle_usb_in_broadcast_streams_0 = {
    /* source     sink            transform     pfeature_table */
    NULL,        NULL,       NULL,       stream_feature_list_ble_audio_dongle_usb_in_broadcast_0
};

extern stream_feature_list_t stream_feature_list_ble_audio_dongle_usb_in_broadcast_1[];
CONNECTION_IF g_ble_audio_dongle_usb_in_broadcast_streams_1 = {
    /* source     sink            transform     pfeature_table */
    NULL,        NULL,       NULL,       stream_feature_list_ble_audio_dongle_usb_in_broadcast_1
};

extern stream_feature_list_t stream_feature_list_ble_audio_dongle_usb_out_broadcast[];
CONNECTION_IF g_ble_audio_dongle_usb_out_broadcast_streams = {
    /* source     sink            transform     pfeature_table */
    NULL,        NULL,       NULL,       stream_feature_list_ble_audio_dongle_usb_out_broadcast
};
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

CONNECTION_IF *port_audio_transmitter_get_connection_if(audio_transmitter_scenario_t scenario_id, audio_transmitter_scenario_sub_id_t sub_id)
{
    CONNECTION_IF *application_ptr = NULL;

    if (scenario_id == AUDIO_TRANSMITTER_A2DP_SOURCE) {


    }
#ifdef MTK_MULTI_MIC_STREAM_ENABLE
    else if (scenario_id == AUDIO_TRANSMITTER_MULTI_MIC_STREAM) {
        application_ptr = &g_multi_mic_streams[sub_id.multimic_id];
    }
#endif
#ifdef MTK_SENSOR_SOURCE_ENABLE
    else if (scenario_id == AUDIO_TRANSMITTER_GSENSOR){
        application_ptr = &sensor_src_if[sub_id.gsensor_id];
    }
#endif
#if defined(MTK_GAMING_MODE_HEADSET) || defined(AIR_GAMING_MODE_DONGLE_ENABLE)
    else if (scenario_id == AUDIO_TRANSMITTER_GAMING_MODE) {

        if(sub_id.gamingmode_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0)
        {
            application_ptr = &g_usb_in_broadcast_streams_0;
        }
        else if(sub_id.gamingmode_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1)
        {
            application_ptr = &g_usb_in_broadcast_streams_1;
        }
        else if(sub_id.gamingmode_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT)
        {
            application_ptr = &g_usb_out_broadcast_streams;
        }else if(sub_id.gamingmode_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET)
        {
            application_ptr = &g_game_headset_ul_streams;
        }
    }
#endif /* MTK_GAMING_MODE_HEADSET || AIR_GAMING_MODE_DONGLE_ENABLE */
#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
    else if (scenario_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE) {

        if(sub_id.ble_audio_dongle_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0)
        {
            application_ptr = &g_ble_audio_dongle_usb_in_broadcast_streams_0;
        }
        else if(sub_id.ble_audio_dongle_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1)
        {
            application_ptr = &g_ble_audio_dongle_usb_in_broadcast_streams_1;
        }
        else if(sub_id.ble_audio_dongle_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT)
        {
            application_ptr = &g_ble_audio_dongle_usb_out_broadcast_streams;
        }
    }
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */
#if defined(MTK_ANC_SURROUND_MONITOR_ENABLE)
    else if (scenario_id == AUDIO_TRANSMITTER_ANC_MONITOR_STREAM){
        application_ptr = &sensor_anc_monitor_if;
    }
#endif
    else if (scenario_id == AUDIO_TRANSMITTER_TEST) {
        if(sub_id.test_id == AUDIO_TRANSMITTER_TEST_AUDIO_LOOPBACK){
#if defined(MTK_AUDIO_LOOPBACK_TEST_ENABLE)
            application_ptr = &audio_loopback_test_if;
            stream_feature_configure_resolution((stream_feature_list_ptr_t)application_ptr->pfeature_table, RESOLUTION_32BIT, CONFIG_DECODER);
#endif
        }
    }
#if defined(MTK_TDM_ENABLE)
    else if (scenario_id == AUDIO_TRANSMITTER_TDM){
        application_ptr = &tdm_if;
        stream_feature_configure_resolution((stream_feature_list_ptr_t)application_ptr->pfeature_table, RESOLUTION_32BIT, CONFIG_DECODER);

        if ((tdm_if.source != 0) && (tdm_if.sink != 0)) {
            tdm_if.source->param.audio.linein_scenario_flag = 1;
            tdm_if.sink->param.audio.linein_scenario_flag = 1;
            Source_Audio_BufferInfo_Rst(tdm_if.source, 0);
            Sink_Audio_BufferInfo_Rst(tdm_if.sink, 0);
        }
    }
#endif
#if defined(AIR_WIRED_AUDIO_ENABLE)
    else if (scenario_id == AUDIO_TRANSMITTER_WIRED_AUDIO){
        //if(sub_id.test_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT){
        //    DSP_MW_LOG_E("[audio_transmitter] get connection_if AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT", 0);
        //    DSP_ALG_UpdateEscoRxMode(mSBC);
        //    application_ptr = &g_usb_out_local_streams;
        //}
        //else
        if((sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0)||(sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1)){
            if(((sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0)&&(g_application_ptr_usb_in_0 == NULL))
            ||((sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1)&&(g_application_ptr_usb_in_1 == NULL)))
            {
                if(wired_audio_get_usb_audio_start_number() == 0){
                    DSP_MW_LOG_I("[audio_transmitter] get connection_if AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0", 0);
                    application_ptr = &g_usb_in_local_streams_0;

                }
                else if(wired_audio_get_usb_audio_start_number() > 0){
                    DSP_MW_LOG_I("[audio_transmitter] get connection_if AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1", 0);
                    application_ptr = &g_usb_in_local_streams_1;
                }
                if(sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0){
                    g_application_ptr_usb_in_0 = application_ptr;
                }
                else if(sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1){
                    g_application_ptr_usb_in_1 = application_ptr;
                }
            }
            else{
                if(sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0){
                    application_ptr = g_application_ptr_usb_in_0;
                }
                else if(sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1){
                    application_ptr = g_application_ptr_usb_in_1;
                }
            }
        }
        else if((sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT) || (sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT) || (sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_OUT_MASTER)){
            DSP_MW_LOG_E("[audio_transmitter] get connection_if AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT/AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT/AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_OUT_MASTER", 0);
            DSP_ALG_UpdateEscoRxMode(mSBC);
            if(sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT) {
                application_ptr = &g_usb_out_local_streams;
            } else {
                 application_ptr = &g_line_out_local_streams;
            }
        }
        else if((sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_IN) || (sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_MASTER) || (sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_SLAVE)){
            DSP_MW_LOG_E("[audio_transmitter] get connection_if AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_IN", 0);
            application_ptr = &g_line_in_local_streams;
        }
    }
#endif /*AIR_WIRED_AUDIO_ENABLE*/
#if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
    else if (scenario_id == AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH) {
        if (sub_id.advanced_passthrough_id == AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH_HEARING_AID)
        {
            application_ptr = &advanced_passthrough_if;
        }
    }
#endif /* AIR_ADVANCED_PASSTHROUGH_ENABLE */

    if (!application_ptr) {
        DSP_MW_LOG_E("[audio_transmitter] get connection_if failed", 0);
        assert(0);
    }
    return application_ptr;
}

#ifdef  AIR_AUDIO_HW_LOOPBACK_ENABLE
afe_loopback_param_t dsp_audio_hw_loopback_spk;
afe_loopback_param_t dsp_audio_hw_loopack_mic;
afe_loopback_param_t dsp_audio_hw_loopack_linein;
void audio_hw_loopback_open(hal_ccni_message_t msg,audio_transmitter_scenario_sub_id_t sub_id)
{
    afe_loopback_param_t *dsp_audio_hw_loopback;
    mcu2dsp_open_param_p open_param;
    open_param = (mcu2dsp_open_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    if(sub_id.audio_hw_loopback_id == AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK_I2S0_TO_DAC){
        dsp_audio_hw_loopback = &dsp_audio_hw_loopback_spk;
    }
    else if(sub_id.audio_hw_loopback_id == AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK_ADC_TO_I2S0){
        dsp_audio_hw_loopback = &dsp_audio_hw_loopack_mic;
    }
    else if(sub_id.audio_hw_loopback_id == AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK_LINEIN_TO_I2S2){
        dsp_audio_hw_loopback = &dsp_audio_hw_loopack_linein;
    }
    else{
        DSP_MW_LOG_E("[audio_transmitter] audio hw loopback open fail,scenario id is not found:%d", 1, sub_id.audio_hw_loopback_id);
    }
        dsp_audio_hw_loopback->in_device = open_param->stream_in_param.afe.audio_device;
        dsp_audio_hw_loopback->in_interface = open_param->stream_in_param.afe.audio_interface;
        dsp_audio_hw_loopback->in_misc_parms.I2sClkSourceType = open_param->stream_in_param.afe.misc_parms;
        dsp_audio_hw_loopback->out_device = open_param->stream_out_param.afe.audio_device;
        dsp_audio_hw_loopback->out_interface = open_param->stream_out_param.afe.audio_interface;
        dsp_audio_hw_loopback->out_misc_parms.I2sClkSourceType = open_param->stream_out_param.afe.misc_parms;
        dsp_audio_hw_loopback->sample_rate = open_param->stream_out_param.afe.sampling_rate;
        dsp_audio_hw_loopback->with_hw_gain = open_param->stream_out_param.afe.hw_gain;
        dsp_audio_hw_loopback->stream_channel = open_param->stream_out_param.afe.stream_channel;
        dsp_audio_hw_loopback->format = open_param->stream_out_param.afe.format;
        if(dsp_audio_hw_loopback->in_device & HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER){
            dsp_audio_hw_loopback->device_handle_in.i2s_master.i2s_interface = open_param->stream_in_param.afe.audio_interface;
        }else if(dsp_audio_hw_loopback->in_device & (HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL|HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_L|HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_R| \
        HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL|HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_L|HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_R)){
            dsp_audio_hw_loopback->device_handle_in.digital_mic.bias_voltage[0] = open_param->stream_in_param.afe.bias_voltage[0];
            dsp_audio_hw_loopback->device_handle_in.digital_mic.bias_voltage[1] = open_param->stream_in_param.afe.bias_voltage[1];
            dsp_audio_hw_loopback->device_handle_in.digital_mic.bias_voltage[2] = open_param->stream_in_param.afe.bias_voltage[2];
            dsp_audio_hw_loopback->device_handle_in.digital_mic.bias_voltage[3] = open_param->stream_in_param.afe.bias_voltage[3];
            dsp_audio_hw_loopback->device_handle_in.digital_mic.bias_voltage[4] = open_param->stream_in_param.afe.bias_voltage[4];
            dsp_audio_hw_loopback->in_misc_parms.MicbiasSourceType = open_param->stream_in_param.afe.misc_parms;
            dsp_audio_hw_loopback->device_handle_in.analog_mic.mic_interface = open_param->stream_in_param.afe.audio_interface;
            dsp_audio_hw_loopback->device_handle_in.analog_mic.adc_parameter.performance = open_param->stream_in_param.afe.performance;
            dsp_audio_hw_loopback->device_handle_in.analog_mic.bias_select = open_param->stream_in_param.afe.bias_select;
            dsp_audio_hw_loopback->device_handle_in.analog_mic.iir_filter = open_param->stream_in_param.afe.iir_filter[0];
            dsp_audio_hw_loopback->device_handle_in.analog_mic.adc_parameter.adc_mode = open_param->stream_in_param.afe.adc_mode;
        }else if(dsp_audio_hw_loopback->in_device & HAL_AUDIO_CONTROL_DEVICE_LINE_IN_DUAL){
            dsp_audio_hw_loopback->device_handle_in.linein.adc_parameter.performance = open_param->stream_in_param.afe.performance;
            dsp_audio_hw_loopback->device_handle_in.linein.bias_voltage[0] = open_param->stream_in_param.afe.bias_voltage[0];
            dsp_audio_hw_loopback->device_handle_in.linein.bias_voltage[1] = open_param->stream_in_param.afe.bias_voltage[1];
            dsp_audio_hw_loopback->device_handle_in.linein.bias_voltage[2] = open_param->stream_in_param.afe.bias_voltage[2];
            dsp_audio_hw_loopback->device_handle_in.linein.bias_voltage[3] = open_param->stream_in_param.afe.bias_voltage[3];
            dsp_audio_hw_loopback->device_handle_in.linein.bias_voltage[4] = open_param->stream_in_param.afe.bias_voltage[4];
            dsp_audio_hw_loopback->device_handle_in.linein.bias_select = open_param->stream_in_param.afe.bias_select;
            dsp_audio_hw_loopback->device_handle_in.linein.iir_filter = open_param->stream_in_param.afe.iir_filter[0];
            dsp_audio_hw_loopback->device_handle_in.linein.adc_parameter.adc_mode = open_param->stream_in_param.afe.adc_mode;
        }
        dsp_audio_hw_loopback->device_handle_out.dac.with_high_performance = open_param->stream_out_param.afe.performance;
        DSP_MW_LOG_I("dsp_audio_hw_loopback in_device %d in_interface %d in_misc_parms %d out_device %d out_interface %d out_misc_parms %d sample_rate %d with_hw_gain %d stream_channel %d format %d adc_mode %d",11,
        dsp_audio_hw_loopback->in_device,dsp_audio_hw_loopback->in_interface,dsp_audio_hw_loopback->in_misc_parms,dsp_audio_hw_loopback->out_device,dsp_audio_hw_loopback->out_interface,
        dsp_audio_hw_loopback->out_misc_parms,dsp_audio_hw_loopback->sample_rate,dsp_audio_hw_loopback->with_hw_gain,dsp_audio_hw_loopback->stream_channel,dsp_audio_hw_loopback->format,
        dsp_audio_hw_loopback->device_handle_in.analog_mic.adc_parameter.adc_mode);
        afe_set_loopback_enable(HAL_AUDIO_CONTROL_ON, dsp_audio_hw_loopback);
        if(sub_id.audio_hw_loopback_id == AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK_I2S0_TO_DAC){
            audio_hw_loopback_echo_enable(HAL_AUDIO_CONTROL_ON);
        }
}

void audio_hw_loopback_close(hal_ccni_message_t msg,audio_transmitter_scenario_sub_id_t sub_id)
{
    UNUSED(msg);
    if(sub_id.audio_hw_loopback_id == AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK_I2S0_TO_DAC){
        afe_set_loopback_enable(HAL_AUDIO_CONTROL_OFF, &dsp_audio_hw_loopback_spk);
        audio_hw_loopback_echo_enable(HAL_AUDIO_CONTROL_OFF);
    }
    else if(sub_id.audio_hw_loopback_id == AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK_LINEIN_TO_I2S2){
        afe_set_loopback_enable(HAL_AUDIO_CONTROL_OFF, &dsp_audio_hw_loopack_linein);
    }
    else if(sub_id.audio_hw_loopback_id == AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK_ADC_TO_I2S0){
        afe_set_loopback_enable(HAL_AUDIO_CONTROL_OFF, &dsp_audio_hw_loopack_mic);
    }
    else{
        DSP_MW_LOG_E("[audio_transmitter] audio hw loopback close fail,scenario id is not found:%d", 1, sub_id.audio_hw_loopback_id);
    }
}
#endif
void audio_transmitter_open(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    CONNECTION_IF *application_ptr;
    mcu2dsp_open_param_p open_param;
    audio_transmitter_scenario_t scenario_id;
    audio_transmitter_scenario_sub_id_t sub_id;

    UNUSED(ack);

    scenario_id = (msg.ccni_message[0] >> 8) & 0xFF;
    sub_id.scenario_id = (msg.ccni_message[0] & 0xFF);
    DSP_MW_LOG_I("[audio_transmitter] open, scenario type %d, scenario sub id %d\r\n", 2, scenario_id, sub_id.scenario_id);
#ifdef  AIR_AUDIO_HW_LOOPBACK_ENABLE
    if(scenario_id == AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK){
        DSP_MW_LOG_I("[audio_transmitter] audio hw loopback open", 0);
        audio_hw_loopback_open(msg,sub_id);
        return;
    }
#endif
    application_ptr = port_audio_transmitter_get_connection_if(scenario_id, sub_id);

    open_param = (mcu2dsp_open_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    application_ptr->source     = dsp_open_stream_in(open_param);
    application_ptr->sink       = dsp_open_stream_out(open_param);
    application_ptr->transform = NULL;

    DSP_Callback_PreloaderConfig(application_ptr->pfeature_table);

    /* do scenario's portable open opeartion here */
    extern void port_audio_transmitter_open(audio_transmitter_scenario_t scenario_id, audio_transmitter_scenario_sub_id_t sub_id, mcu2dsp_open_param_p open_param, SOURCE source, SINK sink);
    port_audio_transmitter_open(scenario_id, sub_id, open_param, application_ptr->source, application_ptr->sink);

    DSP_MW_LOG_I("[audio_transmitter] open finish", 0);
}

void audio_transmitter_start(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    CONNECTION_IF *application_ptr;
    uint32_t gpt_timer;
    mcu2dsp_start_param_p start_param;
    audio_transmitter_scenario_t scenario_id;
    audio_transmitter_scenario_sub_id_t sub_id;

    UNUSED(ack);

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_timer);

    scenario_id = (msg.ccni_message[0] >> 8) & 0xFF;
    sub_id.scenario_id  = (msg.ccni_message[0] & 0xFF);
    DSP_MW_LOG_I("[audio_transmitter] start, gpt_timer %d, scenario type %d, scenario sub id %d\r\n", 3, gpt_timer, scenario_id, sub_id);
#ifdef  AIR_AUDIO_HW_LOOPBACK_ENABLE
    if(scenario_id == AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK){
        DSP_MW_LOG_I("[audio_transmitter] audio hw loopback start", 0);
        return;
    }
#endif
    application_ptr = port_audio_transmitter_get_connection_if(scenario_id, sub_id);

    start_param = (mcu2dsp_start_param_p)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    dsp_start_stream_in (start_param, application_ptr->source);
    dsp_start_stream_out(start_param, application_ptr->sink);

    application_ptr->transform = TrasformAudio2Audio(application_ptr->source, application_ptr->sink, application_ptr->pfeature_table);
    if (application_ptr->transform == NULL) {
        DSP_MW_LOG_E("[audio_transmitter] start fail", 0);
        assert(0);
    }

    /* do scenario's portable start opeartion here */
    extern void port_audio_transmitter_start(audio_transmitter_scenario_t scenario_id, audio_transmitter_scenario_sub_id_t sub_id, mcu2dsp_start_param_p start_param, SOURCE source, SINK sink);
    port_audio_transmitter_start(scenario_id, sub_id, start_param, application_ptr->source, application_ptr->sink);

    DSP_MW_LOG_I("[audio_transmitter] start finish", 0);
}

void audio_transmitter_stop(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    CONNECTION_IF *application_ptr;
    audio_transmitter_scenario_t scenario_id;
    audio_transmitter_scenario_sub_id_t sub_id;

    UNUSED(ack);

    scenario_id = (msg.ccni_message[0] >> 8) & 0xFF;
    sub_id.scenario_id  = (msg.ccni_message[0] & 0xFF);
    DSP_MW_LOG_I("[audio_transmitter] stop, scenario type %d, scenario id %d\r\n", 2, scenario_id, sub_id);
#ifdef  AIR_AUDIO_HW_LOOPBACK_ENABLE
    if(scenario_id == AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK){
        DSP_MW_LOG_I("[audio_transmitter] audio hw loopback stop", 0);
        aud_msg_ack(0x8c03, FALSE);
        return;
    }
#endif
    application_ptr = port_audio_transmitter_get_connection_if(scenario_id, sub_id);

    if (application_ptr->transform != NULL) {
        StreamDSPClose(application_ptr->transform->source, application_ptr->transform->sink, (msg.ccni_message[0] >> 16) | 0x8000);
    }
    application_ptr->transform = NULL;

    /* do scenario's portable stop opeartion here */
    extern void port_audio_transmitter_stop(audio_transmitter_scenario_t scenario_id, audio_transmitter_scenario_sub_id_t sub_id, SOURCE source, SINK sink);
    port_audio_transmitter_stop(scenario_id, sub_id, application_ptr->source, application_ptr->sink);

    DSP_MW_LOG_I("[audio_transmitter] stop finish", 0);
}

void audio_transmitter_close(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    CONNECTION_IF *application_ptr;
    audio_transmitter_scenario_t scenario_id;
    audio_transmitter_scenario_sub_id_t sub_id;

    UNUSED(ack);

    scenario_id = (msg.ccni_message[0] >> 8) & 0xFF;
    sub_id.scenario_id  = (msg.ccni_message[0] & 0xFF);
    DSP_MW_LOG_I("[audio_transmitter] close, scenario type %d, scenario id %d\r\n", 2, scenario_id, sub_id);
#ifdef  AIR_AUDIO_HW_LOOPBACK_ENABLE
    if(scenario_id == AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK){
        DSP_MW_LOG_I("[audio_transmitter] audio hw loopback close", 0);
        audio_hw_loopback_close(msg,sub_id);
        return;
    }
#endif
    application_ptr = port_audio_transmitter_get_connection_if(scenario_id, sub_id);

    /* do scenario's portable close opeartion here */
    extern void port_audio_transmitter_close(audio_transmitter_scenario_t scenario_id, audio_transmitter_scenario_sub_id_t sub_id, SOURCE source, SINK sink);
    port_audio_transmitter_close(scenario_id, sub_id, application_ptr->source, application_ptr->sink);

    SourceClose(application_ptr->source);
    application_ptr->source = NULL;
    SinkClose(application_ptr->sink);
    application_ptr->sink = NULL;
    DSP_PIC_FeatureDeinit(application_ptr->pfeature_table);

    DSP_MW_LOG_I("[audio_transmitter] close finish", 0);
}

void audio_transmitter_config(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    CONNECTION_IF *application_ptr;
    audio_transmitter_scenario_t scenario_id;
    audio_transmitter_scenario_sub_id_t sub_id;
    void *config_param;

    UNUSED(ack);

    scenario_id = (msg.ccni_message[0] >> 8) & 0xFF;
    sub_id.scenario_id  = (msg.ccni_message[0] & 0xFF);
    DSP_MW_LOG_I("[audio transmitter]: config, scenario type %d, scenario id %d\r\n", 2, scenario_id, sub_id);

    application_ptr = port_audio_transmitter_get_connection_if(scenario_id, sub_id);

    config_param = (void *)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);

    extern bool port_audio_transmitter_scenario_config(audio_transmitter_scenario_t scenario_id, audio_transmitter_scenario_sub_id_t sub_id, void *config_param);
    port_audio_transmitter_scenario_config(scenario_id, sub_id, config_param);

    DSP_MW_LOG_I("[audio transmitter]: config finish", 0);
}

void audio_transmitter_suspend(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    CONNECTION_IF *application_ptr;
    audio_transmitter_scenario_t scenario_id;
    audio_transmitter_scenario_sub_id_t sub_id;

    UNUSED(ack);

    scenario_id = (msg.ccni_message[0] >> 8) & 0xFF;
    sub_id.scenario_id  = (msg.ccni_message[0] & 0xFF);
    DSP_MW_LOG_I("[audio_transmitter] suspend, scenario type %d, scenario id %d\r\n", 2, scenario_id, sub_id);

    application_ptr = port_audio_transmitter_get_connection_if(scenario_id, sub_id);
#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
    dsp_trigger_suspend(application_ptr->source, application_ptr->sink);
#endif
    //TODO
    DSP_MW_LOG_I("[audio_transmitter] suspend finish", 0);
}

void audio_transmitter_resume(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    CONNECTION_IF *application_ptr;
    audio_transmitter_scenario_t scenario_id;
    audio_transmitter_scenario_sub_id_t sub_id;

    UNUSED(ack);

    scenario_id = (msg.ccni_message[0] >> 8) & 0xFF;
    sub_id.scenario_id  = (msg.ccni_message[0] & 0xFF);
    DSP_MW_LOG_I("[audio_transmitter] resume, scenario type %d, scenario id %d\r\n", 2, scenario_id, sub_id);

    application_ptr = port_audio_transmitter_get_connection_if(scenario_id, sub_id);
#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
    dsp_detachable_config(application_ptr,msg);
    dsp_trigger_resume(application_ptr->source, application_ptr->sink);
#endif
    //TODO
    DSP_MW_LOG_I("[audio_transmitter] resume finish", 0);
}

void audio_transmitter_send_message(audio_transmitter_scenario_t scenario_type, audio_transmitter_scenario_sub_id_t scenario_id, uint32_t message)
{
    hal_ccni_message_t msg;

    memset((void *)&msg, 0, sizeof(hal_ccni_message_t));
    msg.ccni_message[0] = (MSG_DSP2MCU_AUDIO_TRANSMITTER_DATA_DIRECT << 16) | (scenario_type << 8) | scenario_id.scenario_id;
    msg.ccni_message[1] = message;
    aud_msg_tx_handler(msg, 0, FALSE);

    DSP_MW_LOG_I("[audio_transmitter] send message, scenario type %d, scenario id %d, message 0x%08x", 3, scenario_type, scenario_id, message);
}
#ifdef MTK_GAMING_MODE_HEADSET
void gaming_headset_uplink_enable_irq(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(msg);
    UNUSED(ack);

    hal_audio_trigger_start_parameter_t start_parameter;
    start_parameter.memory_select = HAL_AUDIO_MEMORY_UL_VUL1;
    start_parameter.enable = true;
    DSP_MW_LOG_I("[audio transmitter] Enable vul interrupt", 0);
    hal_audio_set_value((hal_audio_set_value_parameter_t *)&start_parameter, HAL_AUDIO_SET_TRIGGER_MEMORY_START);
}
#endif /* MTK_GAMING_MODE_HEADSET */
#endif

#ifdef MTK_ANC_ENABLE
extern void anc_passthru_stop_callback(void);
extern void anc_passthru_switch_type_callback(void);
#endif
void DSP_LOCAL_TX_HANDEL(hal_ccni_message_t msg)
{
    switch(msg.ccni_message[0] & 0xFFFF){
    #ifdef MTK_ANC_ENABLE
        case 0x0E06:{
            if(msg.ccni_message[1] == 0x0){
                anc_passthru_stop_callback();
            }else if (msg.ccni_message[1] == 0x1){
                anc_passthru_switch_type_callback();
            }
            break;
        }
    #endif
        default:
            break;
    }
}

#ifdef MTK_ANC_ENABLE
mcu2dsp_open_adda_param_t g_audio_adda_param;
void DSP_AUDIO_ADDA_NOTIFY(uint32_t memory_addr)
{
    mcu2dsp_open_adda_param_p adda_param_p;
    adda_param_p = (mcu2dsp_open_adda_param_p)hal_memview_cm4_to_dsp0(memory_addr);
    memcpy(&g_audio_adda_param, adda_param_p, sizeof(mcu2dsp_open_adda_param_t));
}

hal_audio_device_t DSP_OPEN_DAC(bool enable)
{
    hal_audio_device_t open_device = 0;
    hal_audio_device_parameter_t *device_handle;
    device_handle = (hal_audio_device_parameter_t *)pvPortMalloc(sizeof(hal_audio_device_parameter_t));
    if (device_handle != NULL) {
        memset(device_handle, 0, sizeof(hal_audio_device_parameter_t));
        device_handle->dac.audio_device = g_audio_adda_param.dac_setting.audio_device;
        device_handle->dac.rate         = g_audio_adda_param.dac_setting.sampling_rate;
        device_handle->dac.dc_compensation_value  = afe.stream_out.dc_compensation_value;
        device_handle->dac.with_force_change_rate = false;
        device_handle->dac.dac_mode = g_audio_adda_param.dac_setting.dl_dac_mode;
        device_handle->dac.with_high_performance = g_audio_adda_param.dac_setting.performance;
        hal_audio_set_device(device_handle, device_handle->common.audio_device, enable ? HAL_AUDIO_CONTROL_ON : HAL_AUDIO_CONTROL_OFF);
        if (enable) {
            open_device = device_handle->common.audio_device;
        } else {
            open_device = 0;
        }
        vPortFree(device_handle);
    } else {
        DSP_MW_LOG_E("[ANC API] Error!!! malloc fail for anc_enable_dac", 0);
    }
    return open_device;
}

void DSP_OPEN_ADC(hal_audio_control_t *device, hal_audio_device_interface_t *dev_interface, hal_audio_device_parameter_t *device_handle)
{
    uint8_t index = 0;
    if (*device & HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL) {
        device_handle->digital_mic.audio_device  = *device;
        device_handle->digital_mic.mic_interface = *dev_interface;
        device_handle->digital_mic.rate            = g_audio_adda_param.adc_setting.sampling_rate;
        device_handle->digital_mic.bias_select     = g_audio_adda_param.adc_setting.bias_select;
        device_handle->digital_mic.bias_voltage[0] = g_audio_adda_param.adc_setting.bias_voltage[0];
        device_handle->digital_mic.bias_voltage[1] = g_audio_adda_param.adc_setting.bias_voltage[1];
        device_handle->digital_mic.bias_voltage[2] = g_audio_adda_param.adc_setting.bias_voltage[2];
        device_handle->digital_mic.bias_voltage[3] = g_audio_adda_param.adc_setting.bias_voltage[3];
        device_handle->digital_mic.bias_voltage[4] = g_audio_adda_param.adc_setting.bias_voltage[4];
        device_handle->digital_mic.with_external_bias = g_audio_adda_param.adc_setting.with_external_bias;
        device_handle->digital_mic.with_bias_lowpower = g_audio_adda_param.adc_setting.with_bias_lowpower;
        device_handle->digital_mic.bias1_2_with_LDO0  = g_audio_adda_param.adc_setting.bias1_2_with_LDO0;
        switch(device_handle->digital_mic.audio_device){
            case HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL:
            case HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_L:
                index = (device_handle->analog_mic.mic_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1)?0:((device_handle->analog_mic.mic_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2)?2:4);
                device_handle->digital_mic.dmic_selection = g_audio_adda_param.adc_setting.dmic_selection[index];
            break;
            case HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_R:
                index = (device_handle->analog_mic.mic_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1)?1:((device_handle->analog_mic.mic_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2)?3:5);
                device_handle->digital_mic.dmic_selection = g_audio_adda_param.adc_setting.dmic_selection[index];
            break;
            default:
                device_handle->digital_mic.dmic_selection = HAL_AUDIO_DMIC_GPIO_DMIC0;
            break;
        }
        switch(device_handle->digital_mic.mic_interface){
            case HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1:
                device_handle->digital_mic.iir_filter = g_audio_adda_param.adc_setting.iir_filter[0];
            break;
            case HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2:
                device_handle->digital_mic.iir_filter = g_audio_adda_param.adc_setting.iir_filter[1];
            break;
            case HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3:
                device_handle->digital_mic.iir_filter = g_audio_adda_param.adc_setting.iir_filter[2];
            break;
            default:
                 device_handle->digital_mic.iir_filter = HAL_AUDIO_UL_IIR_5HZ_AT_48KHZ;
            break;
        }
    } else {
        device_handle->analog_mic.audio_device  = *device;
        device_handle->analog_mic.mic_interface = *dev_interface;
        device_handle->analog_mic.rate            = g_audio_adda_param.adc_setting.sampling_rate;
        device_handle->analog_mic.bias_select     = g_audio_adda_param.adc_setting.bias_select;
        device_handle->analog_mic.bias_voltage[0] = g_audio_adda_param.adc_setting.bias_voltage[0];
        device_handle->analog_mic.bias_voltage[1] = g_audio_adda_param.adc_setting.bias_voltage[1];
        device_handle->analog_mic.bias_voltage[2] = g_audio_adda_param.adc_setting.bias_voltage[2];
        device_handle->analog_mic.bias_voltage[3] = g_audio_adda_param.adc_setting.bias_voltage[3];
        device_handle->analog_mic.bias_voltage[4] = g_audio_adda_param.adc_setting.bias_voltage[4];
        device_handle->analog_mic.with_external_bias        = g_audio_adda_param.adc_setting.with_external_bias;
        device_handle->analog_mic.adc_parameter.performance = g_audio_adda_param.adc_setting.performance;
        device_handle->analog_mic.with_bias_lowpower        = g_audio_adda_param.adc_setting.with_bias_lowpower;
        device_handle->analog_mic.bias1_2_with_LDO0         = g_audio_adda_param.adc_setting.bias1_2_with_LDO0;
        switch(device_handle->analog_mic.audio_device){
            case HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL:
            case HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_L:
                index = (device_handle->analog_mic.mic_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1)?0:((device_handle->analog_mic.mic_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2)?2:4);
                device_handle->analog_mic.adc_parameter.adc_mode = g_audio_adda_param.adc_setting.ul_adc_mode[index];
            break;
            case HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_R:
                index = (device_handle->analog_mic.mic_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1)?1:((device_handle->analog_mic.mic_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2)?3:5);
                device_handle->analog_mic.adc_parameter.adc_mode = g_audio_adda_param.adc_setting.ul_adc_mode[index];
            break;
            default:
                device_handle->analog_mic.adc_parameter.adc_mode = HAL_AUDIO_ANALOG_INPUT_ACC10K;
            break;
        }
        switch(device_handle->analog_mic.mic_interface){
            case HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1:
                device_handle->analog_mic.iir_filter = g_audio_adda_param.adc_setting.iir_filter[0];
            break;
            case HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2:
                device_handle->analog_mic.iir_filter = g_audio_adda_param.adc_setting.iir_filter[1];
            break;
            case HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3:
                device_handle->analog_mic.iir_filter = g_audio_adda_param.adc_setting.iir_filter[2];
            break;
            default:
                 device_handle->analog_mic.iir_filter = HAL_AUDIO_UL_IIR_25HZ_AT_48KHZ;
            break;
        }
    }
}
#endif

#ifdef MTK_SLT_AUDIO_HW
void AUDIO_SLT_START(hal_ccni_message_t msg, hal_ccni_message_t *ack){
    uint32_t result_total;
    uint32_t rate;
    //audio_slt_enable(48000);
    DSP_MW_LOG_I("SLT rate= %d",1,msg.ccni_message[1]);
    rate = msg.ccni_message[1];

    result_total = audio_slt_test_case(rate);

    DSP_MW_LOG_I("AUDIO_SLT_START",0);
#if 1
    //hal_ccni_message_t msg;
    memset((void *)&msg, 0, sizeof(hal_ccni_message_t));
    msg.ccni_message[0] = 0x70 << 16 | 0x80000000;
    DSP_MW_LOG_I("result_total 0x%x",1,result_total);
    msg.ccni_message[1] = result_total;//0x123;
    aud_msg_tx_handler(msg, 0, FALSE);
#endif

}
#endif

#ifdef MTK_AUDIO_LOOPBACK_TEST_ENABLE
void DSP_AUDIO_LOOPBACK_TEST_CONFIG(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    DSP_MW_LOG_I("[AUDIO LOOPBACK TEST] CONFIG", 0);
    uint32_t addr = (uint32_t)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    audio_loopback_test_inform_result_addr(addr);
    Audio_setting->Audio_source.Pga_mux = HAL_AUDIO_LOOPBACK_TEST_PGA_NULL;
}
#endif

// for volume setting sync
static timer_list_node_t *g_sync_timer_list = NULL;
static aud_msg_cb_node_t *g_dsp_request_sync_cb_front = NULL;
static uint32_t gpt_sync_timer_handle = 0;
hal_gpt_callback_t dsp_audio_request_sync_timer_callback(void *user_data);

// porting function
static aud_msg_cb_node_t *_msg_queue_get_middle(aud_msg_cb_node_t *front, aud_msg_cb_node_t *rear)
{
    if (front == NULL) {
        return NULL;
    }

    aud_msg_cb_node_t *step1 = front;
    aud_msg_cb_node_t *step2 = front->next;

    while (step2 != rear) {
        step2 = step2->next;
        if (step2 != rear) {
            step1 = step1->next;
            step2 = step2->next;
        }
    }

    return step1;
}

static aud_msg_cb_info_t *_msg_queue_search(aud_msg_cb_node_t *front, uint16_t ID)
{
    /* Simple binary search */

    aud_msg_cb_node_t *cb = NULL;
    aud_msg_cb_node_t *rear = NULL;

    do {
        cb = _msg_queue_get_middle(front, rear);

        if (cb == NULL) {
            return NULL;
        }
        else if (cb->cb_info.msg_id == ID)
            return &(cb->cb_info);
        else if (cb->cb_info.msg_id < ID)
            front = cb->next;
        else
            rear = cb;

    } while (front != rear);

    return NULL;
}
static void dsp_audio_msg_ack(uint32_t msg_id, bool from_isr)
{
    // send ACK to cm4 side
    uint32_t ack_msg = 0;
    ack_msg = (uint16_t)msg_id;
    ack_msg |= ((MSG_DSP2MCU_AUDIO_SYNC_DONE | 0x8000) << 16);
    DTM_enqueue(DTM_EVENT_ID_AUDIO_SYNC_END, ack_msg, from_isr);
    DSP_MW_LOG_E("[DSP SYNC] enqueue to dtm task, id [0x%x]\r\n", 1, ack_msg);
}
// msg_callback and ack the msg id
static void dsp_audio_msg_queue_handle(uint32_t msg_id, cm4_dsp_sync_param_t *param)
{
    dsp_sync_callback_t callback;
    // msg.ccni_message[0] structure: [CM42DSP_SYNC_ID 16bit]  [scenario_id 8bit]  [action_id 8bit]
    uint32_t scenario_id = (msg_id & 0xff00)>>8;
    uint32_t action_id = msg_id & 0xff; // the last 8-bit
    aud_msg_cb_info_t *cb = _msg_queue_search(g_dsp_request_sync_cb_front, scenario_id); // [WARNING]
    if(cb != NULL) {
        if(cb->cb_func != NULL){
            callback = cb->cb_func;
            callback(action_id, &(param->vol_gain_info));
        } else {
            // error
            DSP_MW_LOG_E("[DSP SYNC] id [0x%x] [0x%x] no related callback\r\n", 2, scenario_id, action_id);
        }
    } else {
        DSP_MW_LOG_E("[DSP SYNC] id [0x%x] [0x%x] no related register\r\n", 2, scenario_id, action_id);
    }
}

// gpt count compare
static int8_t gpt_count_compare(uint32_t count1, uint32_t count2)
{
    uint32_t tmp1 = count1 > count2 ? count1 : count2;
    uint32_t tmp2 = count1 > count2 ? count2 : count1;
    uint32_t delt = tmp1 - tmp2;
    int8_t sign = delt > 0x7fffffff ? -1 : 1;
    if (count1 > count2) {
        return 1 * sign;
    } else if (count1 < count2) {
        return -1 * sign;
    } else {
        return 0;
    }
}

static aud_msg_status_t dsp_audio_request_sync_timer_list_insert(timer_list_info_t *info)
{
    timer_list_node_t *cur = g_sync_timer_list;
    timer_list_node_t *tmp = NULL;
    if (cur == NULL) {
        timer_list_node_t *cb = (timer_list_node_t *)pvPortMalloc(sizeof(timer_list_node_t));
        if (cb == NULL) {
            DSP_MW_LOG_E("[DSP SYNC] timer_list_node_t malloc failed 1!!\r\n", 0);
            return AUDIO_MSG_STATUS_ERROR;
        }
        memcpy(&(cb->info), info, sizeof(timer_list_info_t));
        cb->next = NULL;
        g_sync_timer_list = cb;
        return AUDIO_MSG_STATUS_OK;
    }
    while((cur->next != NULL) && (info->mirror_count > cur->next->info.mirror_count)) {   /* Search the the closest node */
        cur = cur->next;
    }

    /* Replace or insert the node */
    timer_list_node_t *cb = (timer_list_node_t *)pvPortMalloc(sizeof(timer_list_node_t));
    if (cb == NULL) {
        DSP_MW_LOG_E("[DSP SYNC] timer_list_node_t malloc failed 2!!\r\n", 0);
        return AUDIO_MSG_STATUS_ERROR;
    }
    memcpy(&(cb->info), info, sizeof(timer_list_info_t));

    if(info->mirror_count > cur->info.mirror_count) {
        tmp = cur->next;
        cur->next = cb;
        cb->next = tmp;
    } else {
        tmp = cur;
        cur = cb;
        cb->next = tmp;
    }

    /*  Updated the front of queue pointer */
    if (g_sync_timer_list->info.mirror_count >= cb->info.mirror_count) { // For the same count, latest is the first!
                                                                               // And there is memory leak without the modification.
        g_sync_timer_list = cb;
    }
    return AUDIO_MSG_STATUS_OK;
}

// count the item of timer list
static uint32_t dsp_audio_request_sync_timer_list_get_length(void)
{
    uint32_t cnt = 0;
    timer_list_node_t *timer = (timer_list_node_t *)g_sync_timer_list;
    while (timer) {
        cnt++;
        timer = timer->next;
    }
    return cnt;
}

void dsp_audio_request_sync_process(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    uint8_t              trigger_flag = 0; // excute callback immediately
    uint32_t             savedmask    = 0; // interrupt mask
    uint32_t             curr_cnt     = 0;
    uint32_t             curr_cnt_tmp = 0;
    int32_t              time_out     = 0;
    hal_gpt_status_t     gpt_status   = HAL_GPT_STATUS_OK;
    cm4_dsp_sync_param_t *para_addr   = NULL;
    // for show log
    uint32_t             disable_int_time = 0;
    uint32_t             enable_int_time  = 0;
    uint32_t             total_time       = 0;
    timer_list_info_t info;
    memset(&info, 0, sizeof(timer_list_info_t));
#ifdef MTK_AUDIO_DSP_SYNC_REQUEST_GPIO_DEBUG
    DSP_MW_LOG_I("[DSP SYNC] init gpio test", 0);
    hal_gpio_set_direction(HAL_GPIO_23, 1);
    hal_gpio_disable_pull(HAL_GPIO_23);
    hal_pinmux_set_function(HAL_GPIO_23,0);
    hal_gpio_set_output(HAL_GPIO_23, 0);
#endif /* MTK_AUDIO_DSP_SYNC_REQUEST_GPIO_DEBUG */
    /* -1- parse msg info --------------------------------------------------------------------*/
    uint32_t msg_id      = (uint32_t)msg.ccni_message[0];
    info.msg_id          = msg_id;
    para_addr            = (cm4_dsp_sync_param_t *)hal_memview_cm4_to_dsp0(msg.ccni_message[1]); // dsp must remap the address
    memcpy(&info.param, para_addr, sizeof(cm4_dsp_sync_param_t));
    // dsp_audio_msg_ack(info.msg_id, false);
    /* -2- check the time --------------------------------------------------------------------*/
//#ifdef MTK_AUDIO_DSP_SYNC_REQUEST_DEBUG
    DSP_MW_LOG_I("[DSP SYNC] dsp receive info: id0 = 0x%x, para_addr = 0x%x, channel = %d, vol_gain = %d, t_cnt = %u\r\n", 5,
                    msg_id, para_addr, info.param.vol_gain_info.gain_select, info.param.vol_gain_info.gain, info.param.gpt_count);
//#endif
    hal_nvic_save_and_set_interrupt_mask(&savedmask);
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
    curr_cnt_tmp = curr_cnt;
    disable_int_time = curr_cnt;
    time_out = info.param.gpt_count - curr_cnt;
    if (time_out < AUDIO_DSP_SYNC_MIN_TIME_OUT) { // < 1ms
        // assert(0);
        trigger_flag = 1;
        goto ERROR_HANDLE;
    }
    if (time_out > 5000000) { // 5s, Abnormal Case!
        DSP_MW_LOG_W("[DSP SYNC] ATTENTION: delay time is so big!", 0);
        // do nothing
    }
    uint64_t deviation = time_out;
    info.mirror_count  = deviation + curr_cnt;
    /* -3- append timer list ------------------------------------------------------------------*/
    if (dsp_audio_request_sync_timer_list_insert(&info) != AUDIO_MSG_STATUS_OK) {
        trigger_flag |= 1 << 1;
        goto ERROR_HANDLE;
    }

    /* -4- re-start timer ---------------------------------------------------------------------*/
    if (info.param.gpt_count == g_sync_timer_list->info.param.gpt_count) {
        // restart timer
        if (gpt_sync_timer_handle == 0) {
            gpt_status = hal_gpt_sw_get_timer(&gpt_sync_timer_handle);
            if (gpt_status != HAL_GPT_STATUS_OK) {
                trigger_flag |= 1 << 2;
                DSP_MW_LOG_W("[DSP SYNC] get gpt_status = %d\r\n", 1, gpt_status);
                goto ERROR_HANDLE;
            }
        } else {
            gpt_status = hal_gpt_sw_stop_timer_us(gpt_sync_timer_handle);
            if (gpt_status != HAL_GPT_STATUS_OK) {
                // trigger_flag |= 1 << 3;
                // goto ERROR_HANDLE;
            }
        }
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
        time_out = info.param.gpt_count - curr_cnt - AUDIO_DSP_SYNC_FIXED_POLLING_TIME; // [TODO] check over flow
        // avoid negative number
        if (time_out < 0) {
            time_out = 0;
        }
        gpt_status = hal_gpt_sw_start_timer_us(gpt_sync_timer_handle, time_out, (hal_gpt_callback_t)dsp_audio_request_sync_timer_callback, NULL);
        if (gpt_status != HAL_GPT_STATUS_OK) {
            trigger_flag |= 1 << 4;
            DSP_MW_LOG_W("[DSP SYNC] start gpt_status = %d\r\n", 1, gpt_status);
            goto ERROR_HANDLE;
        } else {
            // if (g_sync_timer_list != NULL) {
            //     if (g_sync_timer_list->next == NULL) {
            //         // boost the frequency
            //         dvfs_lock_control("DSP_SYNC", DVFS_78M_SPEED, DVFS_LOCK);
            //     }
            // }
        }
    }
ERROR_HANDLE:
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &enable_int_time);
    hal_nvic_restore_interrupt_mask(savedmask);
    hal_gpt_get_duration_count(disable_int_time, enable_int_time, &total_time);
    DSP_MW_LOG_I("[DSP SYNC] msg process, exit critical region! total time = %d\r\n", 1, total_time);
    /* -5- excute callback immediately -----------------------------------------------------------*/
    if (trigger_flag != 0) {
        // excute callback immediately, search the related scenario's callback
        dsp_audio_msg_queue_handle(info.msg_id, &(info.param));
        dsp_audio_msg_ack(info.msg_id, false);
        DSP_MW_LOG_W("[DSP SYNC] abnormal condition occur, error = %d\r\n", 1, trigger_flag);
        if (trigger_flag & 0x11100) { // avoid memory leak
            timer_list_node_t *tmp = g_sync_timer_list->next;
            vPortFree(g_sync_timer_list);
            g_sync_timer_list = tmp;
        }
    }
    uint32_t number = dsp_audio_request_sync_timer_list_get_length();
    if (g_sync_timer_list != NULL) { // show detail information
        DSP_MW_LOG_I("[DSP SYNC] timer list  list_len = %d, msg0 = 0x%x, vol_ch = %d, vol_gain = %d, t_cnt = %u, c_cnt = %u, t_out = %d, m_cnt = %u\r\n", 8,
                        number, g_sync_timer_list->info.msg_id, g_sync_timer_list->info.param.vol_gain_info.gain_select, g_sync_timer_list->info.param.vol_gain_info.gain,
                        g_sync_timer_list->info.param.gpt_count, curr_cnt_tmp, time_out, g_sync_timer_list->info.mirror_count);
    } else {
        DSP_MW_LOG_I("[DSP SYNC] timer list msg0 = 0x%x, vol_ch = %d, vol_gain = %d, t_cnt = %u, c_cnt = %u, t_out = %d, m_cnt = %u, number = 0\r\n",
                        7, info.msg_id, info.param.vol_gain_info.gain_select, info.param.vol_gain_info.gain,  info.param.gpt_count, curr_cnt_tmp, time_out, info.mirror_count);
    }
}

aud_msg_status_t dsp_audio_request_sync_register_callback(dsp_sync_request_scenario_id_t request_scenario_id, dsp_sync_callback_t *sync_callback)
{
    uint8_t ack_opt = 0; // means ignore this parameter
    aud_msg_cb_node_t *cur = g_dsp_request_sync_cb_front;
    aud_msg_cb_node_t *tmp = NULL;

    if (cur == NULL) {   /* Nothing in the queue */
        aud_msg_cb_node_t *cb = (aud_msg_cb_node_t *)pvPortMalloc(sizeof(aud_msg_cb_node_t));
        if(cb == NULL){
            DSP_MW_LOG_E("DSP SYNC CB malloc failed 1!!\r\n", 0);
            return AUDIO_MSG_STATUS_ERROR;
        }
        cb->cb_info.msg_id = request_scenario_id;
        cb->cb_info.cb_func = sync_callback;
        cb->cb_info.ack_option = ack_opt;
        cb->next = NULL;
        g_dsp_request_sync_cb_front = cb;
        return AUDIO_MSG_STATUS_OK;
    }

    while((cur->next != NULL) && (request_scenario_id >= cur->next->cb_info.msg_id)) {   /* Search the the closest node */
        cur = cur->next;
    }

    /* Replace or insert the node */
    if (request_scenario_id == cur->cb_info.msg_id) {
        cur->cb_info.cb_func = sync_callback;
        cur->cb_info.ack_option = ack_opt;
        return AUDIO_MSG_STATUS_OK;
    } else {
        aud_msg_cb_node_t *cb = (aud_msg_cb_node_t *)pvPortMalloc(sizeof(aud_msg_cb_node_t));
        if (cb == NULL) {
            DSP_MW_LOG_E("DSP SYNC CB malloc failed 2!!\r\n", 0);
            return AUDIO_MSG_STATUS_ERROR;
        }
        cb->cb_info.msg_id = request_scenario_id;
        cb->cb_info.cb_func = sync_callback;
        cb->cb_info.ack_option = ack_opt;

        if(request_scenario_id > cur->cb_info.msg_id){
            tmp = cur->next;
            cur->next = cb;
            cb->next = tmp;
        } else {
            tmp = cur;
            cur = cb;
            cb->next = tmp;
        }

        /*  Updated the front of queue pointer */
        if (g_dsp_request_sync_cb_front->cb_info.msg_id >= cb->cb_info.msg_id) {
            g_dsp_request_sync_cb_front = cb;
        }
        if (g_dsp_request_sync_cb_front->cb_info.msg_id == cb->cb_info.msg_id) { // one id can only has one callback func.
            DSP_MW_LOG_E("[DSP SYNC] ERROR: register callback, ID 0x%x 0x%x", 2, g_dsp_request_sync_cb_front->cb_info.msg_id,
                cb->cb_info.msg_id);
            assert(0);
            return AUDIO_MSG_STATUS_ERROR;
        }
    }
    return AUDIO_MSG_STATUS_OK;
}

hal_gpt_callback_t dsp_audio_request_sync_timer_callback(void *user_data)
{
    UNUSED(user_data);
    timer_list_info_t info;
    uint32_t savedmask = 0;
    uint32_t curr_cnt  = 0;
    uint32_t time_out  = 0;
    // for show log
    uint32_t disable_int_time = 0;
    uint32_t enable_int_time  = 0;
    uint32_t total_time       = 0;
    timer_list_node_t *tmp    = NULL; // buffer the next timer list pointer before freeing the first item pointer of timer list
    memset(&info, 0, sizeof(timer_list_info_t));
    hal_nvic_save_and_set_interrupt_mask(&savedmask); // enter cirtical code region
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &disable_int_time);
#ifdef MTK_AUDIO_DSP_SYNC_REQUEST_GPIO_DEBUG
    hal_gpio_set_output(HAL_GPIO_2, 0);
#endif /* MTK_AUDIO_DSP_SYNC_REQUEST_GPIO_DEBUG */
LOOP:
#ifdef MTK_AUDIO_DSP_SYNC_REQUEST_DEBUG
    DSP_MW_LOG_I("[DSP SYNC] loop 0x%x %d %d %d\r\n", 4, g_sync_timer_list->info.msg_id, g_sync_timer_list->info.param.vol_gain_info.gain_select,
                    g_sync_timer_list->info.param.vol_gain_info.gain, g_sync_timer_list->info.param.gpt_count);
#endif /* MTK_AUDIO_DSP_SYNC_REQUEST_DEBUG */
    /* -1- free expired timer ----------------------------------------------------------------*/
    tmp = g_sync_timer_list->next;
    memcpy(&info, &(g_sync_timer_list->info), sizeof(timer_list_info_t));
    /* -2- polling tick and excute expired timer's callback  ---------------------------------*/
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
    if (info.param.gpt_count > curr_cnt) { // gpt register does not overflow
#ifdef MTK_AUDIO_DSP_SYNC_REQUEST_DEBUG
    DSP_MW_LOG_I("[DSP SYNC] trigger %d curr_cnt %u  tar %d", 3, __LINE__, curr_cnt, info.param.gpt_count);
#endif /* MTK_AUDIO_DSP_SYNC_REQUEST_DEBUG */
        while (1) {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
            if(curr_cnt >= info.param.gpt_count) { // expire at time
                break;
            }
        }
    } else if (curr_cnt - info.param.gpt_count > 0x7fffffff) { // gpt register overflow
#ifdef MTK_AUDIO_DSP_SYNC_REQUEST_DEBUG
    DSP_MW_LOG_I("[DSP SYNC] trigger %d curr_cnt %u  tar %d", 3, __LINE__, curr_cnt, info.param.gpt_count);
#endif /* MTK_AUDIO_DSP_SYNC_REQUEST_DEBUG */
        while (1) {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
            if(curr_cnt >= info.param.gpt_count) { // expire at time
                if((curr_cnt & 0x80000000) == 0x0) {
                    break;
                }
            }
        }
    } else {
        DSP_MW_LOG_E("[DSP SYNC]Warning: already expire\r\n", 0);
        // assert(0);
    }
    // callback and ack
    dsp_audio_msg_queue_handle(info.msg_id, &(info.param));
    dsp_audio_msg_ack(info.msg_id, true);
    DSP_MW_LOG_I("[DSP SYNC] Trigger, polling end! c_cnt = %u, t_cnt = %u\r\n", 2, curr_cnt, info.param.gpt_count);
    vPortFree(g_sync_timer_list);
    g_sync_timer_list = tmp;
#ifdef MTK_AUDIO_DSP_SYNC_REQUEST_DEBUG
    if (g_sync_timer_list != NULL) { // show detail information
        DSP_MW_LOG_I("[DSP SYNC] next timer list msg0 = 0x%x, vol_ch = %d, vol_gain = %d, t_cnt = %u, list_len = %d\r\n", 5, g_sync_timer_list->info.msg_id,
                    g_sync_timer_list->info.param.vol_gain_info.gain_select, g_sync_timer_list->info.param.vol_gain_info.gain, g_sync_timer_list->info.param.gpt_count,
                    dsp_audio_request_sync_timer_list_get_length());
    }
#endif /* MTK_AUDIO_DSP_SYNC_REQUEST_DEBUG */
    /* -3- check and start next timer  -------------------------------------------------------*/
    if (g_sync_timer_list != NULL) {
        // while (g_sync_timer_list->next) {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
            if (gpt_count_compare(g_sync_timer_list->info.param.gpt_count, curr_cnt) != 1) {
                // trigger immedately
                DSP_MW_LOG_D("[DSP SYNC] Trigger immediately, goto loop\r\n", 0);
                goto LOOP; // about 30-40us delay
            } else {
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
                time_out = g_sync_timer_list->info.param.gpt_count - curr_cnt - AUDIO_DSP_SYNC_FIXED_POLLING_TIME; // [TODO] check overflow
                int8_t res = gpt_count_compare(g_sync_timer_list->info.param.gpt_count, curr_cnt + AUDIO_DSP_SYNC_REQUEST_MIN_REQUEST_INTERVAL);
                if (res == 1) {
                    // start timer
                    hal_gpt_status_t gpt_status = hal_gpt_sw_start_timer_us(gpt_sync_timer_handle, time_out, (hal_gpt_callback_t) dsp_audio_request_sync_timer_callback, NULL);
                    if (gpt_status != HAL_GPT_STATUS_OK) { // [TODO]
                        DSP_MW_LOG_E("[DSP SYNC] timer start error\r\n", 0);
                        // assert
                    }
                    // break;
                } else {
                    // start polling
                    DSP_MW_LOG_D("[DSP SYNC] goto loop\r\n", 0);
                    goto LOOP; // ATTENTION: maybe exist some abnormal sitution
                }
            }
        //}
    }
#ifdef MTK_AUDIO_DSP_SYNC_REQUEST_DEBUG
    DSP_MW_LOG_I("[DSP SYNC] exit loop\r\n", 0);
#endif /* MTK_AUDIO_DSP_SYNC_REQUEST_DEBUG */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &enable_int_time);
    hal_nvic_restore_interrupt_mask(savedmask);
    hal_gpt_get_duration_count(disable_int_time, enable_int_time, &total_time);
    DSP_MW_LOG_I("[DSP SYNC] timer callback, exit citical region! total time = %d\r\n", 1, total_time);
    // release gpt timer handle
    if (g_sync_timer_list == NULL) {
        // down frequency
        // dvfs_lock_control("DSP_SYNC", DVFS_78M_SPEED, DVFS_UNLOCK); // NO API
        if (hal_gpt_sw_free_timer(gpt_sync_timer_handle) != HAL_GPT_STATUS_OK) {
            DSP_MW_LOG_E("[DSP SYNC] gpt timer release error\r\n", 0);
        } else {
            DSP_MW_LOG_I("[DSP SYNC] release timer, timer list len = %d\r\n", 1, dsp_audio_request_sync_timer_list_get_length());
        }
        gpt_sync_timer_handle = 0;
    }
    return 0;
}

#ifdef AIR_AUDIO_HARDWARE_ENABLE
extern void dsp_sync_callback_a2dp(dsp_sync_request_action_id_t request_action_id, void *user_data);
extern void dsp_sync_callback_hfp(dsp_sync_request_action_id_t request_action_id, void *user_data);
extern void dsp_sync_callback_vp(dsp_sync_request_action_id_t request_action_id, void *user_data);

void dsp_audio_request_sync_initialization(void)
{
    //Register callback
#if defined(AIR_HFP_SYNC_START_ENABLE) || defined(AIR_HFP_SYNC_STOP_ENABLE)
    dsp_audio_request_sync_register_callback(MSG_MCU2DSP_HFP_SYNC_REQUEST, (dsp_sync_callback_t *)dsp_sync_callback_hfp);
#endif
#if defined(AIR_A2DP_SYNC_START_ENABLE) || defined(AIR_A2DP_SYNC_STOP_ENABLE)
    dsp_audio_request_sync_register_callback(MSG_MCU2DSP_A2DP_SYNC_REQUEST, (dsp_sync_callback_t *)dsp_sync_callback_a2dp);
#endif
    dsp_audio_request_sync_register_callback(MSG_MCU2DSP_VP_SYNC_REQUEST, (dsp_sync_callback_t *)dsp_sync_callback_vp);
}
#endif

#ifdef MTK_ANC_ENABLE
log_create_module(anc, PRINT_LEVEL_INFO);

ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_0[]  = LOG_DEBUG_PREFIX(anc) "ANC Start , type: %d\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_1[]  = LOG_DEBUG_PREFIX(anc) "ANC Start Finish, ret : %d, Time_use 32K(%d) 1M(%d) ADC(%d) DAC(%d) ANC(%d)\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_2[]  = LOG_DEBUG_PREFIX(anc) "=============debug ANC ============\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_3[]  = LOG_DEBUG_PREFIX(anc) "0x%04X: 0x%08x 0x%08x 0x%08x 0x%08x\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_4[]  = LOG_DEBUG_PREFIX(anc) "=============debug ANC END ============\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_5[]  = LOG_DEBUG_PREFIX(anc) "Error!!! anc_get_device_by_mic_mask with NULL pointer";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_6[]  = LOG_DEBUG_PREFIX(anc) "[ANC_API] Enable:%d ANC MIC mic_mask:0x%x device:0x%x interface:0x%x device:0x%x interface:0x%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_7[]  = LOG_DEBUG_PREFIX(anc) "[ANC_API] Error!!! malloc fail for source_device";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_8[]  = LOG_DEBUG_PREFIX(anc) "[ANC API] Error!!! device malloc fail for connect_ul4_spdif";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_9[]  = LOG_DEBUG_PREFIX(anc) "[ANC API] Error!!! path malloc fail for connect_ul4_spdif";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_10[] = LOG_DEBUG_PREFIX(anc) "[ANC_API]Headset project cannot support ANC_FEATURE_1.\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_11[] = LOG_DEBUG_PREFIX(anc) "[ANC_API]feature_param.mix %x %x %x %x 0x%x.\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_12[] = LOG_DEBUG_PREFIX(anc) "[ANC_API]Headset project cannot support FF ANC_FEATURE_1.\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_13[] = LOG_DEBUG_PREFIX(anc) "[ANC_API]feature_param.mix %x %x %x %x 0x%x.\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_14[] = LOG_DEBUG_PREFIX(anc) "[ANC_API]Headset project cannot support FB ANC_FEATURE_1.\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_15[] = LOG_DEBUG_PREFIX(anc) "[anc_passthru_get_param] Error!! SMOOTH_SWITCH_FILTER ERROR_2\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_16[] = LOG_DEBUG_PREFIX(anc) "[anc_passthru_get_param] Error!! Un-supported type:%d\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_17[] = LOG_DEBUG_PREFIX(anc) "[anc_passthru_get_param] type:%d, filter_mask_l:0x%x, filter_mask_r:0x%x sram_mask:0x%x\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_18[] = LOG_DEBUG_PREFIX(anc) "[anc_passthru_set_param] Error!! ANC_REG_ID_RAMP_GAIN_USER_DEFINE. type:%d\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_19[] = LOG_DEBUG_PREFIX(anc) "[anc_passthru_set_param] ANC_REG_ID_RAMP_GAIN_EXTEND. extend_ramp:(%d)(%d)\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_20[] = LOG_DEBUG_PREFIX(anc) "[anc_passthru_set_param] Error!! ANC not enable. Set REG id:%d\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_21[] = LOG_DEBUG_PREFIX(anc) "[ANC API] anc_passthru_switch_type_callback";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_22[] = LOG_DEBUG_PREFIX(anc) "[ANC_API] Switch type: XX->%d , need to disable MIC mask:0x%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_23[] = LOG_DEBUG_PREFIX(anc) "[ANC API] anc_passthru_switch_type_callback finish";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_24[] = LOG_DEBUG_PREFIX(anc) "[ANC API] disable anc/passthru finish";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_25[] = LOG_DEBUG_PREFIX(anc) "[ANC API] anc_passthru_stop_callback finish";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_26[] = LOG_DEBUG_PREFIX(anc) "[ANC API] anc on DONE, notify CM4\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_27[] = LOG_DEBUG_PREFIX(anc) "[ANC API][notify_callback] Ramp has been stable gain_1(0x%x) gain_2(0x%x)\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_28[] = LOG_DEBUG_PREFIX(anc) "[ANC API][notify_callback] Ramp has been not stable\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_29[] = LOG_DEBUG_PREFIX(anc) "[ANC API][notify_callback] band_num_1(%d), out_gain_1(0x%x)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_30[] = LOG_DEBUG_PREFIX(anc) "[ANC API][notify_callback] band_num_2(%d), out_gain_2(0x%x)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_31[] = LOG_DEBUG_PREFIX(anc) "[ANC API][notify_callback] filter parameter change done\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_32[] = LOG_DEBUG_PREFIX(anc) "[ANC API][notify_callback] Event(%d) ERROR\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_33[] = LOG_DEBUG_PREFIX(anc) "[ANC API][notify_callback][GET] Parameter wasn't reliable.\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_34[] = LOG_DEBUG_PREFIX(anc) "[ANC API][notify_callback][GET] Ramp status(%d) gain_1(0x%x) gain_2(0x%x)\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_35[] = LOG_DEBUG_PREFIX(anc) "[ANC API][notify_callback][GET] band_num_1(%d), out_gain_1(0x%x)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_36[] = LOG_DEBUG_PREFIX(anc) "[ANC API][notify_callback][GET] band_num_2(%d), out_gain_2(0x%x)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_37[] = LOG_DEBUG_PREFIX(anc) "[ANC_API] get ramp mask fail\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_38[] = LOG_DEBUG_PREFIX(anc) "Error!!! malloc fail for switch anc type";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_39[] = LOG_DEBUG_PREFIX(anc) "Error!!! null anc_param for type switch\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_40[] = LOG_DEBUG_PREFIX(anc) "Error!!! Not support user defined type and other type at the same time.\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_41[] = LOG_DEBUG_PREFIX(anc) "[ANC_API] Switch flash_no: %d->%d , need to enable another sram bank: 0x%x->0x%x for filter 0x%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_42[] = LOG_DEBUG_PREFIX(anc) "[ANC_API] Switch type: %d->%d , need to disable ANC filters: 0x%x 0x%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_43[] = LOG_DEBUG_PREFIX(anc) "[ANC_API] Switch type: %d->%d , need to enable MIC mask:0x%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_44[] = LOG_DEBUG_PREFIX(anc) "[ANC_API] Switch type: %d->%d , need to enable ANC filter: 0x%x 0x%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_45[] = LOG_DEBUG_PREFIX(anc) "[ANC_API]Enter audio_anc_passthru_enable.\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_46[] = LOG_DEBUG_PREFIX(anc) "[ANC_API]Wrong flash_no : %d.\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_47[] = LOG_DEBUG_PREFIX(anc) "[ANC_API]Wrong type : %d.\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_48[] = LOG_DEBUG_PREFIX(anc) "[ANC_API]NULL anc nvdm param.\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_49[] = LOG_DEBUG_PREFIX(anc) "[ANC_API]NULL passthru nvdm param.\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_50[] = LOG_DEBUG_PREFIX(anc) "[ANC_API]NULL biquad filter.\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_51[] = LOG_DEBUG_PREFIX(anc) "[ANC_API]NULL biquad filter for headset R.\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_52[] = LOG_DEBUG_PREFIX(anc) "[ANC_API]Enable SMOOTH_SWITCH_FILTER ERROR_1.\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_53[] = LOG_DEBUG_PREFIX(anc) "[ANC_API]Analog setting (0x%x) (0x%x) (0x%x) (0x%x).";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_54[] = LOG_DEBUG_PREFIX(anc) "[ANC_API]Enable malloc fail.\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_55[] = LOG_DEBUG_PREFIX(anc) "[ANC API][DEBUG] anc_passthru_stop_callback finish";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_56[] = LOG_DEBUG_PREFIX(anc) "[ANC API] disable anc/passthru: is turning off with speed mode: %d";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_57[] = LOG_DEBUG_PREFIX(anc) "[ANC API] disable ANC/Passthru FAIL due to malloc FAIL";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_58[] = LOG_DEBUG_PREFIX(anc) "[ANC API] realtime update FAIL due to anc disabled";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_59[] = LOG_DEBUG_PREFIX(anc) "[ANC API] realtime update FAIL due to abnormal filter:%d";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_60[] = LOG_DEBUG_PREFIX(anc) "[ANC API] realtime update FAIL due to filter:%d inactive";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_61[] = LOG_DEBUG_PREFIX(anc) "[ANC API] realtime update FAIL due to abnormal length:%d";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_62[] = LOG_DEBUG_PREFIX(anc) "[ANC API] realtime update FAIL due to malloc FAIL";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_63[] = LOG_DEBUG_PREFIX(anc) "[ANC_API] Realtime update coefficient , need to enable another sram bank: 0x%x->0x%x for filter 0x%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_64[] = LOG_DEBUG_PREFIX(anc) "ANC Stop\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_65[] = LOG_DEBUG_PREFIX(anc) "ANC Stop Finish, ret : %d \r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_66[] = LOG_DEBUG_PREFIX(anc) "dsp_set_anc_param mode:%d \n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_67[] = LOG_DEBUG_PREFIX(anc) "dsp_set_anc_param, reg_id : %d, filter_mask(0x%x), value(0x%x)\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_68[] = LOG_DEBUG_PREFIX(anc) "dsp_set_anc_param, switch_ramp_dly : %d, switch_up_step(%d), switch_dn_step(%d) switch_delay(%d)\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_69[] = LOG_DEBUG_PREFIX(anc) "dsp_set_anc_param ERROR, switch_ramp_dly : %d, switch_up_step(%d), switch_dn_step(%d)\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_70[] = LOG_DEBUG_PREFIX(anc) "ANC_Filter_mask 0x%x\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_71[] = LOG_DEBUG_PREFIX(anc) "ANC set volume msg: 0x%x 0x%x\r\n";
/*hal_audio_anc.c*/
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_72[] = LOG_DEBUG_PREFIX(anc) "DSP - Hal Audio ANC GPT set filter_id = %d!!\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_73[] = LOG_DEBUG_PREFIX(anc) "DSP - Hal Audio ANC free one_shot Timer error!!, error id = %d.\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_74[] = LOG_DEBUG_PREFIX(anc) "hal_anc_reset CTL_0:0x%08x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_75[] = LOG_DEBUG_PREFIX(anc) "Set_notify(%d) status(%d) param_1:0x%x, param_2:0x%x, param_3:0x%x, param_4:0x%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_76[] = LOG_DEBUG_PREFIX(anc) "Set_notify(%d) ch(%d) param_1:(%d)(%d), param_2:(%d)(%d), param_3:(%d)(%d), param_4:(%d)(%d)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_77[] = LOG_DEBUG_PREFIX(anc) "Set_notify(%d) event error.";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_78[] = LOG_DEBUG_PREFIX(anc) "anc set biquad with NULL ptr. filter_id:%d";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_79[] = LOG_DEBUG_PREFIX(anc) "anc set biquad error: invalid filter_id:0x%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_80[] = LOG_DEBUG_PREFIX(anc) "anc set biquad error: use active sram bank. filter_id:0x%x sram_mux:0x%x, active_filter_mask:0x%x active_sram_mux:0x%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_81[] = LOG_DEBUG_PREFIX(anc) "anc set biquad error: invalid band_num:%d";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_82[] = LOG_DEBUG_PREFIX(anc) "hal_anc_set_biquad_coef: filter_id:0x%x sram:0x%x band_num:%d";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_83[] = LOG_DEBUG_PREFIX(anc) "filter[%d-%d] b[%d] 0x%x 0x%x 0x%x 0x%x 0x%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_84[] = LOG_DEBUG_PREFIX(anc) "hal_anc_set_biquad_enable enable:%d filter_id:%d sram_mux:0x%x band_num:%d";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_85[] = LOG_DEBUG_PREFIX(anc) "invalid anc param: bitshift:%d for filter_id:%d";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_86[] = LOG_DEBUG_PREFIX(anc) "hal_anc_set_bit_shift (%d), (0x%x) 0x%08x 0x%08x 0x%08x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_87[] = LOG_DEBUG_PREFIX(anc) "hal_anc_set_dg_gain (%d), (0x%x) 0x%08x 0x%08x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_88[] = LOG_DEBUG_PREFIX(anc) "hal_anc_set_BQ_gain (%d), (0x%x) 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_89[] = LOG_DEBUG_PREFIX(anc) "hal_anc_set_ramp_gain filter:%d gain:%ddB(0x%x), dly:%d step:%d %d";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_90[] = LOG_DEBUG_PREFIX(anc) "hal_anc_set_LM filter:%d 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_91[] = LOG_DEBUG_PREFIX(anc) "hal_anc_set_LM_param filter:%d reg_id: %d 0x%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_92[] = LOG_DEBUG_PREFIX(anc) "hal_anc_set_deq filter:%d delay:%d s_g:%d";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_93[] = LOG_DEBUG_PREFIX(anc) "hal_anc_set_mic filter:%d mic:%d MIC_SET_3=0x%x CTL_0=0x%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_94[] = LOG_DEBUG_PREFIX(anc) "hal_anc_set_deq_mix enable:%d filter_mask:0x%x deq01_dest_ch:%d %d DGAIN_SET_0=0x%x CTL_0=0x%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_95[] = LOG_DEBUG_PREFIX(anc) "hal_anc_set_classg enable:%d filter_mask:0x%x CLASSG_SET_0=0x%08x ";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_96[] = LOG_DEBUG_PREFIX(anc) "hal_anc_set_omix enable:%d filter_mask_l:0x%x filter_mask_r:0x%x OMIX_SET_0:0x%08x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_97[] = LOG_DEBUG_PREFIX(anc) "hal_anc_set_enable enable:%d filter_mask_l:0x%x filter_mask_r:0x%x CTL_0:0x%08x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_98[] = LOG_DEBUG_PREFIX(anc) "hal_anc_set_ul4_path enable:%d ch_select:%d ul_rate:%d CTL_0:0x%08x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_99[] = LOG_DEBUG_PREFIX(anc) "hal_anc_pwd_enable val:0x%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_100[] = LOG_DEBUG_PREFIX(anc) "hal_anc_pwd_enable:%d, index_mask:0x%x threshold:%d";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_101[] = LOG_DEBUG_PREFIX(anc) "hal_anc_pwd_set_threshold SET: ch:%d, %d(dB)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_102[] = LOG_DEBUG_PREFIX(anc) "hal_anc_pwd_set_threshold MOVE AWAY IRQ_status:0x%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_103[] = LOG_DEBUG_PREFIX(anc) "hal_anc_pwd_set_threshold SET OFFSET before";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_104[] = LOG_DEBUG_PREFIX(anc) "hal_anc_pwd_set_threshold SET OFFSET: ch:%d, %d dB";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_105[] = LOG_DEBUG_PREFIX(anc) "hal_anc_pwd_set_threshold SET OFFSET after";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_106[] = LOG_DEBUG_PREFIX(anc) "DSP - Hal Audio ANC get one_shot Timer error!!, error id = %d.\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_107[] = LOG_DEBUG_PREFIX(anc) "DSP - Hal Audio ANC start one_shot Timer error!!, error id = %d.\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_108[] = LOG_DEBUG_PREFIX(anc) "DSP - Hal Audio ANC switch sram bank to 0x%x, filter_id:%d cur_gain:0x%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_109[] = LOG_DEBUG_PREFIX(anc) "[Rdebug]Filter_id(%d) m_mask(0x%x) c_mask(0x%x) dn_gain(0x%x) continue";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_110[] = LOG_DEBUG_PREFIX(anc) "[Rdebug]Filter_id(%d) mask(0x%x) t_gain(%d) c_gain(%d) dn_gain(0x%x)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_111[] = LOG_DEBUG_PREFIX(anc) "DSP - Hal Audio ANC t_service filter_id(%d) mask(0x%x) t_gain(%d) c_gain(0x%x) dn_gain(0x%x)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_112[] = LOG_DEBUG_PREFIX(anc) "DSP - timer_period %d";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_113[] = LOG_DEBUG_PREFIX(anc) "hal_anc_set_ramp_related:(%d)(%d)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_114[] = LOG_DEBUG_PREFIX(anc) "hal_anc_get_ramp_related: ret(%d), (%d)(%d) (%d)(%d)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_115[] = LOG_DEBUG_PREFIX(anc) "Hal Audio ANC t_service: Attack(%d) ch(%d) t_gain(%d) c_gain(0x%x) dn_gain(0x%x)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_116[] = LOG_DEBUG_PREFIX(anc) "Hal Audio ANC t_service: Release(%d) ch(%d) t_gain(%d) c_gain(0x%x) dn_gain(0x%x)";

#if !defined(MTK_DEBUG_LEVEL_NONE)
static log_control_block_t *g_log_control_blocks[] = {
    &LOG_CONTROL_BLOCK_SYMBOL(anc),
};

void anc_port_log_info(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
    va_list ap;
    va_start(ap, arg_cnt);
    log_print_msgid(g_log_control_blocks[index], PRINT_LEVEL_INFO, message, arg_cnt, ap);
    va_end(ap);
}

void anc_port_log_notice(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
    va_list ap;
    va_start(ap, arg_cnt);
    log_print_msgid(g_log_control_blocks[index], PRINT_LEVEL_WARNING, message, arg_cnt, ap);
    va_end(ap);
}

void anc_port_log_error(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
    va_list ap;
    va_start(ap, arg_cnt);
    log_print_msgid(g_log_control_blocks[index], PRINT_LEVEL_ERROR, message, arg_cnt, ap);
    va_end(ap);
}

void anc_port_log_debug(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
    va_list ap;
    va_start(ap, arg_cnt);
    log_print_msgid(g_log_control_blocks[index], PRINT_LEVEL_DEBUG, message, arg_cnt, ap);
    va_end(ap);
}
#else
void anc_port_log_info(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
}
void anc_port_log_notice(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
}
void anc_port_log_error(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
}
void anc_port_log_debug(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
}
void anc_port_log_MP(uint8_t index, uint8_t arg_cnt, uint8_t value1, uint8_t value2, uint8_t value3)
{
}
#endif
#endif
#if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
log_create_module(psap, PRINT_LEVEL_INFO);
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_0[]  = LOG_DEBUG_PREFIX(psap) "hearing-aid feature set param fail, p_nvkey==NULL\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_1[]  = LOG_DEBUG_PREFIX(psap) "hearing-aid feature set param fail, p_AFC_NVKEY==NULL\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_2[]  = LOG_DEBUG_PREFIX(psap) "hearing-aid feature set param fail, p_LD_NR_NVKEY==NULL\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_3[]  = LOG_DEBUG_PREFIX(psap) "hearing-aid feature set param fail, p_AT_AGC_NVKEY==NULL\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_4[]  = LOG_DEBUG_PREFIX(psap) "hearing-aid feature set param fail, p_AT_AGC_DRC_NVKEY==NULL\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_5[]  = LOG_DEBUG_PREFIX(psap) "hearing-aid feature set param fail, p_MISC_NVKEY==NULL\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_6[]  = LOG_DEBUG_PREFIX(psap) "hearing-aid feature set param fail, nvkey 0x%x not found\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_7[]  = LOG_DEBUG_PREFIX(psap) "hearing-aid feature init\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_8[]  = LOG_DEBUG_PREFIX(psap) "hearing-aid sink afe buffer prefill size: %d (samples),ch:%d\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_9[]  = LOG_DEBUG_PREFIX(psap) "hearing-aid MISC NVKEY NULL\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_10[]  = LOG_DEBUG_PREFIX(psap) "hearing-aid gain setting: 0x%x, 0x%x, 0x%x ; afc_cal:%d\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_11[]  = LOG_DEBUG_PREFIX(psap) "hearing-aid AFC NVKEY NULL\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_12[]  = LOG_DEBUG_PREFIX(psap) "hearing-aid LD_NR NVKEY NULL\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_13[]  = LOG_DEBUG_PREFIX(psap) "hearing-aid AT_AGC NVKEY NULL\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_14[]  = LOG_DEBUG_PREFIX(psap) "hearing-aid AT_AGC_DRC NVKEY NULL\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_15[]  = LOG_DEBUG_PREFIX(psap) "hearing-aid feature deinit\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_16[]  = LOG_DEBUG_PREFIX(psap) "hearing-aid stream start, gpt count = 0x%x\r\n";

/*AFC*/
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_50[]  = LOG_DEBUG_PREFIX(psap) "[AFC] Port not enough!\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_51[]  = LOG_DEBUG_PREFIX(psap) "[AFC] Port is NULL!\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_52[]  = LOG_DEBUG_PREFIX(psap) "[AFC] status is error, %d!\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_53[]  = LOG_DEBUG_PREFIX(psap) "[AFC] error channel number, %d, %d!\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_54[]  = LOG_DEBUG_PREFIX(psap) "[AFC] Port is not found!\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_55[]  = LOG_DEBUG_PREFIX(psap) "[AFC] error setting %d, %d, %d!\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_56[]  = LOG_DEBUG_PREFIX(psap) "[AFC][Version 0x%x] init done, %u, %u, %u, %u, %u! cal:%d\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_57[]  = LOG_DEBUG_PREFIX(psap) "[AFC][param] %u, %u, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_58[]  = LOG_DEBUG_PREFIX(psap) "[AFC] finish time %u\r\n";

/*LDNR*/
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_70[]  = LOG_DEBUG_PREFIX(psap) "[LD_NR][BG] ld_nr_background_entry\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_71[]  = LOG_DEBUG_PREFIX(psap) "[LD_NR][BG] find port %d status:%d, enable:%d, pro_en:%d, pro_fr_num:%d, pro_fr_acc:%d\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_72[]  = LOG_DEBUG_PREFIX(psap) "[LD_NR][BG] background process port %d\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_73[]  = LOG_DEBUG_PREFIX(psap) "[LD_NR][BG] proc2 ch%d res:%d, duration:%u\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_74[]  = LOG_DEBUG_PREFIX(psap) "[LD_NR][BG] no user then delete background entry\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_75[]  = LOG_DEBUG_PREFIX(psap) "[LD_NR] Port not enough!\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_76[]  = LOG_DEBUG_PREFIX(psap) "[LD_NR] Port is NULL!\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_77[]  = LOG_DEBUG_PREFIX(psap) "[LD_NR] status is error, %d!\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_78[]  = LOG_DEBUG_PREFIX(psap) "[LD_NR] Port is not found!\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_79[]  = LOG_DEBUG_PREFIX(psap) "[LD_NR] error setting %d, %d, %d!\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_80[]  = LOG_DEBUG_PREFIX(psap) "[LD_NR][Version 0x%x] init done, %u, %u, %u, %u, %u!\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_81[]  = LOG_DEBUG_PREFIX(psap) "[LD_NR] proc1 fr%d res:%d, duration:%u, finish time:%u\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_82[]  = LOG_DEBUG_PREFIX(psap) "[LD_NR] abnormal res:%d, duration:%u\r\n";

/*AGC*/
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_90[]  = LOG_DEBUG_PREFIX(psap) "[AT_AGC] error band_sw!\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_91[]  = LOG_DEBUG_PREFIX(psap) "[AT_AGC] Port not enough!\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_92[]  = LOG_DEBUG_PREFIX(psap) "[AT_AGC] Port is NULL!\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_93[]  = LOG_DEBUG_PREFIX(psap) "[AT_AGC] status is error, %d!\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_94[]  = LOG_DEBUG_PREFIX(psap) "[AT_AGC] Port is not found!\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_95[]  = LOG_DEBUG_PREFIX(psap) "[AT_AGC] error setting %d, %d, %d!\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_96[]  = LOG_DEBUG_PREFIX(psap) "[AT_AGC][Version 0x%x] init done, %u, %u, %u, 0x%x, %u!\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_97[]  = LOG_DEBUG_PREFIX(psap) "[AT_AGC]Target DRC Gain = 0x%x, Current DRC Gain = 0x%x\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_98[]  = LOG_DEBUG_PREFIX(psap) "[AT_AGC]Target DRC Gain = 0x%x, Current DRC Gain = 0x%x\r\n";
ATTR_LOG_STRING_LIB  g_PSAP_msg_id_string_99[]  = LOG_DEBUG_PREFIX(psap) "[AT_AGC] finish time %u\r\n";



#if !defined(MTK_DEBUG_LEVEL_NONE)
static log_control_block_t *g_psap_log_control_blocks[] = {
    &LOG_CONTROL_BLOCK_SYMBOL(psap),
};

void psap_port_log_info(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
    va_list ap;
    va_start(ap, arg_cnt);
    log_print_msgid(g_psap_log_control_blocks[index], PRINT_LEVEL_INFO, message, arg_cnt, ap);
    va_end(ap);
}

void psap_port_log_notice(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
    va_list ap;
    va_start(ap, arg_cnt);
    log_print_msgid(g_psap_log_control_blocks[index], PRINT_LEVEL_WARNING, message, arg_cnt, ap);
    va_end(ap);
}

void psap_port_log_error(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
    va_list ap;
    va_start(ap, arg_cnt);
    log_print_msgid(g_psap_log_control_blocks[index], PRINT_LEVEL_ERROR, message, arg_cnt, ap);
    va_end(ap);
}

void psap_port_log_debug(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
    va_list ap;
    va_start(ap, arg_cnt);
    log_print_msgid(g_psap_log_control_blocks[index], PRINT_LEVEL_DEBUG, message, arg_cnt, ap);
    va_end(ap);
}
#else
void psap_port_log_info(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
}
void psap_port_log_notice(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
}
void psap_port_log_error(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
}
void psap_port_log_debug(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
}
void psap_port_log_MP(uint8_t index, uint8_t arg_cnt, uint8_t value1, uint8_t value2, uint8_t value3)
{
}
#endif

#endif
