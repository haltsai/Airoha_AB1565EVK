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

#ifndef __HAL_AUDIO_DRIVER_H__
#define __HAL_AUDIO_DRIVER_H__

#include "hal_audio.h"
#ifdef HAL_AUDIO_MODULE_ENABLED


#include "hal_audio_control.h"
#include "hal_audio_path.h"
#include "hal_audio_volume.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define AFE_SIDETONE_0DB_REGISTER_VALUE 32767 //(2^15-1)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Type Definitions ///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum {

    AFE_AUDIO_BT_SYNC_DL1       = 0x10000,
    AFE_AUDIO_BT_SYNC_DL2       = 0x20000,
    AFE_AUDIO_BT_SYNC_DL12      = 0x40000,
    AFE_AUDIO_BT_SYNC_DL3       = 0x80000,
    AFE_AUDIO_BT_SYNC_ENABLE    = AFE_AUDIO_BT_SYNC_DL1|AFE_AUDIO_BT_SYNC_DL2|AFE_AUDIO_BT_SYNC_DL12|AFE_AUDIO_BT_SYNC_DL3,

} afe_audio_bt_sync_con0_t;

typedef enum {
    AFE_MEMORY_ALIGN_8BIT_0_24BIT_DATA = 0,
    AFE_MEMORY_ALIGN_24BIT_DATA_8BIT_0 = 1,
} afe_memory_align_t;

typedef enum {
    AFE_MEMORY_BUFFER_MODE_COMPACT = 0,
    AFE_MEMORY_BUFFER_MODE_NORMAL = 1,
} afe_memory_buffer_mode_t;

typedef enum {
    AFE_SPDIF_CLOCK_SOURCE_DCXO     = 0,
    AFE_SPDIF_CLOCK_SOURCE_APLL1,
    AFE_SPDIF_CLOCK_SOURCE_APLL2,
    AFE_SPDIF_CLOCK_SOURCE_DCXO48K,
} afe_spdif_clock_source_t;

typedef enum {
    AFE_SPDIF_CLOCK_DIVIDER_1     = 0,
    AFE_SPDIF_CLOCK_DIVIDER_2,
    AFE_SPDIF_CLOCK_DIVIDER_4,
    AFE_SPDIF_CLOCK_DIVIDER_8,
    AFE_SPDIF_CLOCK_DIVIDER_10,
    AFE_SPDIF_CLOCK_DIVIDER_16,
    AFE_SPDIF_CLOCK_DIVIDER_32,
} afe_spdif_clock_divider_t;

typedef enum {
    AFE_SPDIF_SELECTION_I2S0     = 0,
    AFE_SPDIF_SELECTION_I2S1,
    AFE_SPDIF_SELECTION_I2S2,
} afe_spdif_selection_t;


typedef enum {
    AFE_UPDOWN_RATIO_BY1 = 0,
    AFE_UPDOWN_RATIO_BY2,
    AFE_UPDOWN_RATIO_BY3,
    AFE_UPDOWN_RATIO_BY4,
    AFE_UPDOWN_RATIO_BY6,
    AFE_UPDOWN_RATIO_BY12,
} afe_updown_ratio_t;

typedef enum {
    AFE_DMIC_CLOCK_DEVICE_INTERFACE_1 = 0,
    AFE_DMIC_CLOCK_DEVICE_INTERFACE_2 = 1,
    AFE_DMIC_CLOCK_DEVICE_INTERFACE_3 = 2,
    AFE_DMIC_CLOCK_DEVICE_INTERFACE_4 = 3,
} afe_dmic_clock_select_t;

typedef enum {
    AFE_DMIC_CLOCK_3_25M    = 0,
    AFE_DMIC_CLOCK_1_625M   = 1,
    AFE_DMIC_CLOCK_812_5K   = 2,
    AFE_DMIC_CLOCK_406_25K  = 3,
} afe_dmic_clock_rate_t;
typedef enum {
    AFE_VOLUME_ANALOG_OUTPUT_CLASSD_CAP_VALUE_NEG_6_DB   = 31,
    AFE_VOLUME_ANALOG_OUTPUT_CLASSD_CAP_VALUE_NEG_3_DB   = 15,
    AFE_VOLUME_ANALOG_OUTPUT_CLASSD_CAP_VALUE_POS_0_DB   = 7,
    AFE_VOLUME_ANALOG_OUTPUT_CLASSD_CAP_VALUE_POS_3_DB   = 3,
    AFE_VOLUME_ANALOG_OUTPUT_CLASSD_CAP_VALUE_POS_6_DB   = 1,
} afe_volume_analog_output_cap_value_t;

typedef struct {
    uint32_t in_ratio;
    uint32_t out_ratio;
    const uint32_t *coef;
} afe_src_iir_coefficient_t, *afe_asrc_iir_coefficient_p;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Variables ///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function Prototypes ////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
hal_audio_agent_t hal_device_convert_agent(hal_audio_control_t device, hal_audio_device_interface_t device_interface);

void afe_samplerate_set_i2s_master_samplerate(afe_i2s_id_t i2s_id, uint32_t samplerate);
uint32_t afe_samplerate_get_i2s_master_samplerate(afe_i2s_id_t i2s_id);
void afe_samplerate_set_ul_samplerate(hal_audio_agent_t agent, uint32_t samplerate);
uint32_t afe_samplerate_get_ul_samplerate(hal_audio_agent_t agent);
void afe_samplerate_set_dl_samplerate(uint32_t samplerate);
uint32_t afe_samplerate_get_dl_samplerate(void);

uint32_t hal_samplerate_convert_to_register_value(hal_audio_agent_t agent, uint32_t samplerate);

/*******************************************************************************************
*                                       Memory agent                                       *
********************************************************************************************/
hal_audio_agent_t hal_memory_convert_agent(hal_audio_memory_selection_t memory_select);
bool hal_memory_set_enable_by_memory_selection (hal_audio_memory_selection_t memory_select, hal_audio_control_status_t control);
bool hal_memory_set_enable(hal_audio_agent_t agent, hal_audio_control_status_t control);
bool hal_memory_set_samplerate(hal_audio_agent_t agent, uint32_t samplerate);
bool hal_memory_set_channel(hal_audio_agent_t agent, uint32_t channel);
bool hal_memory_set_format(hal_audio_agent_t agent, hal_audio_pcm_format_t format);
bool hal_memory_set_align(hal_audio_agent_t agent, afe_memory_align_t memory_align);
bool hal_memory_set_buffer_mode(hal_audio_agent_t agent, afe_memory_buffer_mode_t buffer_mode);
bool hal_memory_set_address(hal_audio_memory_parameter_t *handle, hal_audio_agent_t agent);
uint32_t hal_memory_get_address(hal_audio_agent_t agent);
uint32_t hal_memory_get_length(hal_audio_agent_t agent);
uint32_t hal_memory_get_offset(hal_audio_agent_t agent);
hal_audio_irq_audiosys_t hal_memory_convert_audiosys_irq_number(hal_audio_agent_t agent);
hal_audio_agent_t hal_memory_convert_agent_from_audiosys_irq_number(hal_audio_irq_audiosys_t irq_number);
bool hal_memory_set_irq_period(hal_audio_agent_t agent, uint32_t samplerate, uint32_t counter);
bool hal_memory_set_irq_enable(hal_audio_agent_t agent, hal_audio_control_status_t control);
bool hal_memory_set_palyen(hal_audio_agent_t agent, hal_audio_control_status_t control);
uint32_t hal_memory_get_palyen_monitor(hal_audio_agent_t agent);


int32_t hal_src_set_power(afe_asrc_id_t asrc_id, bool on);
bool hal_src_set_iir(afe_asrc_id_t asrc_id, uint32_t input_rate, uint32_t output_rate);
bool hal_src_set_configuration(afe_src_configuration_t *config, bool enable);
bool hal_src_set_continuous(afe_src_configuration_t *config, bool enable);
bool hal_src_set_start(afe_asrc_id_t asrc_id, bool enable);
bool hal_src_start_continuous_mode(afe_asrc_id_t src_id, bool wait_playen, bool enable);
void hal_src_set_input_write_offset(afe_asrc_id_t src_id, uint32_t offset);
void hal_src_set_output_read_offset(afe_asrc_id_t src_id, uint32_t offset);
uint32_t hal_src_get_input_read_offset(afe_asrc_id_t src_id);
uint32_t hal_src_get_output_write_offset(afe_asrc_id_t src_id);
uint32_t hal_src_get_input_base_address(afe_asrc_id_t src_id);
uint32_t hal_src_get_output_base_address(afe_asrc_id_t src_id);
uint32_t hal_src_get_src_input_rate(afe_asrc_id_t src_id);
void hal_src_set_src_input_rate(afe_asrc_id_t src_id, uint32_t register_value);
uint32_t hal_src_get_src_input_sample_count(afe_asrc_id_t src_id);


/*******************************************************************************************
*                                         HW gain                                          *
********************************************************************************************/
bool hal_hw_gain_set_enable(afe_hardware_digital_gain_t gain_select, uint32_t samplerate, bool enable);
bool hal_hw_gain_set_target(afe_hardware_digital_gain_t gain_select, uint32_t gain);
#ifdef AIR_A2DP_DRC_TO_USE_DGAIN_ENABLE
void hal_dgian_to_drc(uint32_t gain);
#endif
bool hal_hw_gain_set_down_step(afe_hardware_digital_gain_t gain_select,uint32_t down_step);
bool hal_hw_gain_set_up_step(afe_hardware_digital_gain_t gain_select,uint32_t up_step);
uint32_t hal_hw_gain_get_target(afe_hardware_digital_gain_t gain_select);
uint32_t hal_hw_gain_get_current_gain(afe_hardware_digital_gain_t gain_select);
bool hal_hw_gain_is_running(afe_hardware_digital_gain_t gain_select);
uint32_t hal_hw_gain_get_sample_rate(afe_hardware_digital_gain_t gain_select);
bool hal_hw_gain_set_current_gain(afe_hardware_digital_gain_t gain_select, uint32_t gain);


void hal_gain_set_analog_output_class_ab(afe_volume_analog_output_gain_value_t gain);
void hal_gain_set_analog_output_class_d(afe_volume_analog_output_gain_value_t gain_l, afe_volume_analog_output_gain_value_t gain_r);
uint32_t hal_gain_get_analog_output(void);
void hal_gain_set_analog_input(afe_hardware_analog_gain_t gain_select, afe_volume_analog_input_gain_value_t gain_l,  afe_volume_analog_input_gain_value_t gain_r);


/*******************************************************************************************
*                                       Sine Generator                                         *
********************************************************************************************/
bool hal_sine_generator_set_samplerate(uint32_t samplerate);
bool hal_sine_generator_set_amplitude(afe_sine_generator_amplitude_t amplitude_divider);
bool hal_sine_generator_set_period(uint32_t period_divider);
bool hal_sine_generator_set_enable(hal_audio_agent_t agent, bool is_input, bool enable);


/*******************************************************************************************
*                                       I2S master                                         *
********************************************************************************************/
afe_i2s_id_t hal_i2s_convert_id(hal_audio_control_t device, hal_audio_device_interface_t device_interface);
afe_i2s_id_t hal_i2s_convert_id_by_agent(hal_audio_agent_t agent);
uint32_t hal_i2s_master_convert_i2s_register(afe_i2s_id_t i2s_id);
bool hal_i2s_master_set_configuration(hal_audio_device_parameter_i2s_master_t *config, afe_i2s_id_t i2s_id);
void hal_i2s_master_enable_apll(afe_i2s_apll_t apll_source, bool enable);
void hal_i2s_master_enable_mclk(afe_i2s_apll_t apll_source, afe_i2s_id_t i2s_id, uint32_t mclk_divider, bool enable);
bool hal_i2s_master_set_loopback(afe_i2s_id_t i2s_id, bool enable);
bool hal_i2s_master_enable(afe_i2s_id_t i2s_id, bool enable);
void hal_i2s_master_gpio_init(afe_i2s_id_t i2s_id);


/*******************************************************************************************
*                                       I2S slave                                         *
********************************************************************************************/
void hal_i2s_slave_set_clock();
bool hal_i2s_slave_set_configuration(hal_audio_device_parameter_i2s_slave_t *config, afe_i2s_id_t i2s_id);
bool hal_i2s_slave_set_power(afe_i2s_id_t i2s_id, bool enable);
bool hal_i2s_slave_set_enable(afe_i2s_id_t i2s_id, bool enable);
bool hal_i2s_slave_set_share_fifo();
void hal_i2s_slave_gpio_init(afe_i2s_id_t i2s_id);

/*******************************************************************************************
*                                          SPDIF                                           *
********************************************************************************************/
bool hal_spdif_set_configuration(hal_audio_device_parameter_spdif_t *config);
bool hal_spdif_enable(bool enable);


/*******************************************************************************************
*                                     Up/Down Sampler                                      *
********************************************************************************************/
bool hal_updown_set_ratio(afe_updown_sampler_id_t updown_id, uint32_t input_rate, uint32_t output_rate);
bool hal_updown_set_input_rate(afe_updown_sampler_id_t updown_id, uint32_t input_rate);
uint32_t hal_updown_get_input_rate(afe_updown_sampler_id_t updown_id);
bool hal_updown_set_output_rate(afe_updown_sampler_id_t updown_id, uint32_t output_rate);
uint32_t hal_updown_get_output_rate(afe_updown_sampler_id_t updown_id);
bool hal_updown_set_enable(afe_updown_sampler_id_t updown_id, bool enable);


/*******************************************************************************************
*                                        tick align                                        *
********************************************************************************************/
bool hal_tick_align_set_updown(afe_updown_sampler_id_t updown_id, hal_audio_path_interconnection_tick_source_t tick_source, bool enable);
bool hal_tick_align_set_hw_gain(afe_hardware_digital_gain_t gain_select, hal_audio_path_interconnection_tick_source_t tick_source, bool enable);
bool hal_tick_align_set_memory_agent(hal_audio_memory_selection_t memory_select, hal_audio_path_interconnection_tick_source_t tick_source, bool enable);
bool hal_tick_align_set_irq(hal_audio_agent_t agent, bool enable);


/*******************************************************************************************
*                                     UL/DL device                                         *
********************************************************************************************/
void hal_audio_adda_set_enable_register(bool enable);

bool hal_audio_ul_set_enable(hal_audio_agent_t agent, bool enable);
bool hal_audio_ul_set_iir(hal_audio_agent_t agent, hal_audio_ul_iir_t iir_filter, bool enable);
uint32_t hal_audio_ul_get_iir(hal_audio_agent_t agent);
bool hal_audio_ul_set_hires(hal_audio_agent_t agent, bool enable);
bool hal_audio_ul_set_dmic_bias(hal_audio_agent_t agent, bool enable);
bool hal_audio_ul_set_dmic_phase(hal_audio_agent_t agent, uint32_t phase_ch1, uint32_t phase_ch2);
bool hal_audio_ul_set_dmic_clock(hal_audio_agent_t agent, afe_dmic_clock_rate_t clock_rate);
bool hal_audio_ul_set_dmic_selection (hal_audio_agent_t agent, hal_audio_dmic_selection_t dmic_selection);
bool hal_audio_ul_set_dmic_enable(hal_audio_agent_t agent, bool enable);
bool hal_audio_ul_set_da_loopback_enable(hal_audio_agent_t agent, bool enable);
bool hal_audio_ul_get_fifo_clock_sel(hal_audio_agent_t agent);
bool hal_audio_ul_reset_fifo(hal_audio_agent_t device_agent, bool enable);
bool hal_audio_ul_set_swap(hal_audio_agent_t device_agent, bool enable);
bool hal_audio_ul_set_inverse(hal_audio_agent_t device_agent, bool enable);
bool hal_audio_ul4_set_loopback(hal_audio_ul_loopback_setting_t loopback_setting, bool enable);

bool hal_audio_dl_set_fifo_swap(bool no_swap);
bool hal_audio_dl_set_sdm(hal_audio_dl_sdm_setting_t sdm_setting, bool enable);
hal_audio_dl_sdm_setting_t hal_audio_dl_get_sdm(void);
bool hal_audio_dl_set_inverse(bool enable);
bool hal_audio_dl_set_src_enable(bool enable);
bool hal_audio_dl_set_hires(hal_audio_agent_t agent, bool enable);
bool hal_audio_dl_set_sdm_enable(bool enable);
bool hal_audio_dl_set_classg(bool enable);
bool hal_audio_dl_set_classd(bool enable);

bool hal_audio_dl_set_classg_monitor(void);
#ifdef AIR_A2DP_NLE_ENABLE
bool hal_audio_dl_set_nle_enable(bool enable);
bool hal_audio_dl_set_nle_gain(bool enable);
#endif
uint32_t hal_audio_dl_get_classab_compensation_value(void);
uint32_t hal_audio_dl_get_classd_compensation_value(void);

/*******************************************************************************************
*                                         SideTone                                         *
********************************************************************************************/
bool hal_sidetone_set_input(hal_audio_agent_t input_agent, bool enable);
bool hal_sidetone_set_output(hal_audio_agent_t output_agent, bool enable);
bool hal_sidetone_set_filter(uint32_t samplerate, uint16_t *p_sidetone_FIR_coef);
bool hal_sidetone_set_enable(bool enable);
uint32_t hal_sidetone_convert_negative_gain_value(int32_t gain);
uint32_t hal_sidetone_convert_positive_gain_value(int32_t gain);
void hal_sidetone_set_gain_by_register_value(int32_t positve_gain_value, int32_t negative_gain_value);
void hal_sidetone_set_volume(int32_t gain);
/*******************************************************************************************
*                                         VOW                                         *
********************************************************************************************/

bool hal_wow_set_config(bool vow_with_hpf,uint32_t snr_threshold,uint8_t alpha_rise,uint32_t mic_selection);
bool hal_wow_power_enable(bool enable);
void spm_mtcmos_vow_on(void);
void spm_mtcmos_vow_off(void);

bool hal_wow_set_dmic(hal_audio_vow_mode_t vow_mode,hal_audio_dmic_selection_t dmic_selection,uint32_t mic_selection,hal_audio_control_status_t control);
bool hal_wow_set_amic(hal_audio_vow_mode_t vow_mode,hal_audio_performance_mode_t performance,afe_analog_select_t analog_select,uint32_t mic_selection,hal_audio_control_status_t control);
void hal_vow_reset(void);

bool hal_wow_set_dma_irq_threshold(uint8_t dma_irq_threshold);
bool hal_wow_get_signal_noise_status(hal_audio_vow_control_t* vow_control);

bool hal_wow_clear_snr_irq_status(hal_audio_vow_control_t vow_control);
bool hal_wow_clear_fifo_irq_status(hal_audio_vow_control_t vow_control);


/*******************************************************************************************
*                                           ANA                                            *
********************************************************************************************/
bool hal_audio_ana_set_global_bias (bool enable);
bool hal_audio_ana_set_bias_configuration (hal_audio_bias_selection_t bias_select, hal_audio_bias_voltage_t bias_voltage, bool is_low_power,bool bias1_2_with_LDO0, bool enable);

bool hal_audio_ana_set_bias_low_power (hal_audio_bias_selection_t bias_select, bool is_low_power);
bool hal_audio_ana_set_dmic_enable (hal_audio_dmic_selection_t dmic_select, bool enable);
bool hal_audio_ana_set_vad_irq_mask (bool mask);
bool hal_audio_ana_set_vad_analog_enable (hal_audio_device_parameter_vad_t *vad_parameter, bool enable);
bool hal_audio_ana_set_vad_digital_enable (hal_audio_device_parameter_vad_t *vad_parameter, bool enable);

bool hal_audio_ana_set_adc0_enable (hal_audio_device_parameter_adc_t *adc_parameter, afe_analog_control_t analog_control, bool enable);
bool hal_audio_ana_set_dac_enable (hal_audio_device_parameter_dac_t *dac_parameter, afe_analog_control_t analog_control, bool enable);
bool hal_audio_ana_set_adc23_enable (hal_audio_device_parameter_adc_t *adc_parameter, afe_analog_control_t analog_control, bool enable);
bool hal_audio_ana_set_adc45_enable (hal_audio_device_parameter_adc_t *adc_parameter, afe_analog_control_t analog_control, bool enable);
void hal_audio_ana_enable_capless_LDO(bool enable);
void hal_audio_ana_enable_ADC_13MCK(afe_analog_select_t analog_select,bool enable);
void hal_audio_ana_micbias_LDO0_set_enable (hal_audio_bias_selection_t bias_select,hal_audio_bias_voltage_t bias_voltage, bool is_low_power, bool enable);

#endif //#ifdef HAL_AUDIO_MODULE_ENABLED
#endif /* __HAL_AUDIO_DRIVER_H__ */
