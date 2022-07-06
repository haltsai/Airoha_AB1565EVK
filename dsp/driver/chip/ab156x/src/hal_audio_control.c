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

#include "hal_audio_control.h"
#include "hal_audio_clock.h"
#include "hal_audio_driver.h"
#include "hal_audio_volume.h"
#include "hal_audio_register.h"
#ifdef ENABLE_HWSRC_CLKSKEW
#include "sink_inter.h"
#endif

#ifdef MTK_ANC_ENABLE
#include "anc_api.h"
#endif

#ifdef AIR_BT_CODEC_BLE_ENABLED
#include "source_inter.h"
#include "transform_inter.h"
#endif

#include <xtensa/tie/xt_hifi2.h>
#include <xtensa/tie/xt_misc.h>
#include <xtensa/tie/xt_mul.h>

#ifdef ENABLE_HWSRC_CLKSKEW
#include "clk_skew.h"
#include "clk_skew_protect.h"
#endif


#ifdef HAL_AUDIO_MODULE_ENABLED
#define I2S_SHARE_CLOCK_INDEX 24

/******************************************************************************
 * External Function Prototypes
 ******************************************************************************/
#ifdef AIR_SIDETONE_ENABLE
extern afe_sidetone_param_t dsp_afe_sidetone;
#endif
extern afe_samplerate_general_t afe_samplerate_convert_samplerate_to_register_value(uint32_t samplerate);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables Declaration //////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
hal_audio_memory_sram_control_t hal_sram_manager;
int32_t hal_audio_agent_user_count[HAL_AUDIO_AGENT_NUMBERS];
hal_audio_device_status_t hal_audio_device_status[HAL_AUDIO_AGENT_DEVICE_NUMBERS];
afe_src_clock_compensation_t afe_src_compensation[AFE_SRC_NUMBER];
afe_analog_channel_control_t afe_analog_control[AFE_ANALOG_NUMBER];
hal_audio_sidetone_control_t sidetone_control;
hal_audio_amp_control_t      amp_control;
hal_audio_vow_control_t      vow_control;
uint16_t                     vow_pre_ch0_noise_msb = 0;
uint16_t                     vow_pre_ch1_noise_msb = 0;
//extern HAL_AUDIO_SEMAPHO_HANDLE audio_device_semaphore;
#if (HAL_AUDIO_VOW_DEBUG)
    HAL_AUDIO_TIMER_HANDLE      vow_debug_timer_handle;
#endif



#if (HAL_AUDIO_VAD_DRIVER)
hal_audio_vad_control_t      vad_control;
#endif

static int16_t afe_control_special_isr_counter;
static int16_t afe_control_adda_counter;
static int16_t afe_control_global_bias_counter;
static int16_t afe_control_bias_counter[HAL_AUDIO_BIAS_SELECT_NUM];
static hal_audio_analog_mdoe_t afe_adc_performance_mode[AFE_ANALOG_NUMBER];
uint32_t pre_src1_empty_gpt_cnt = 0; // use to detect HWSRC 1 underflow during eSCO
#ifdef AIR_A2DP_NLE_ENABLE
static int nle_sample_count;
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functiion Prototype //////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t hal_audio_control_get_agent_count(hal_audio_control_select_parameter_t *audio_select);
uint32_t hal_audio_control_get_audio_count(void);

uint32_t hal_audio_control_get_current_offset(hal_audio_current_offset_parameter_t *offset_handle, hal_audio_get_value_command_t command);
uint32_t hal_audio_control_set_current_offset(hal_audio_current_offset_parameter_t *offset_handle, hal_audio_set_value_command_t command);

bool hal_audio_device_set_inout_cnt(hal_audio_control_t device, hal_audio_control_status_t control);
bool hal_audio_device_set_output_gpio(hal_audio_control_status_t control, bool is_time_up);
bool hal_audio_device_hold_output_gpio(bool is_hold);
U32 hal_audio_device_get_gpio(void);
bool hal_audio_device_set_delay_timer(hal_audio_device_parameter_t *handle, hal_audio_control_t device, hal_audio_control_status_t control);
uint32_t hal_audio_device_get_using_count(hal_audio_device_parameter_t *handle, hal_audio_control_t device);
bool hal_audio_device_distinguish_channel(hal_audio_device_parameter_t *handle, hal_audio_control_t device, hal_audio_control_status_t control);
bool hal_audio_device_change_rate(hal_audio_device_parameter_t *handle, hal_audio_control_t device, hal_audio_control_status_t control);
bool hal_audio_device_set_dac(hal_audio_device_parameter_dac_t *handle, hal_audio_control_t device, hal_audio_control_status_t control);
bool hal_audio_device_set_linein(hal_audio_device_parameter_linein_t *handle, hal_audio_control_t device, hal_audio_control_status_t control);
bool hal_audio_device_set_ul_loopback(hal_audio_device_parameter_loopback_t *handle, hal_audio_control_t device, hal_audio_control_status_t control);
bool hal_audio_device_set_analog_mic(hal_audio_device_parameter_analog_mic_t *handle, hal_audio_control_t device, hal_audio_control_status_t control);
bool hal_audio_device_set_digital_mic(hal_audio_device_parameter_digital_mic_t *handle, hal_audio_control_t device, hal_audio_control_status_t control);
bool hal_audio_device_set_i2s_slave(hal_audio_device_parameter_i2s_slave_t *handle, hal_audio_control_t device, hal_audio_control_status_t control);
bool hal_audio_device_set_i2s_master(hal_audio_device_parameter_i2s_master_t *handle, hal_audio_control_t device, hal_audio_control_status_t control);
bool hal_audio_device_set_spdif(hal_audio_device_parameter_spdif_t *handle, hal_audio_control_t device, hal_audio_control_status_t control);
bool hal_audio_device_set_vad(hal_audio_device_parameter_vad_t *handle, hal_audio_control_t device, hal_audio_control_status_t control);
bool hal_audio_device_set_vow(hal_audio_device_parameter_vow_t *handle, hal_audio_control_t device, hal_audio_control_status_t control);

bool hal_audio_device_set_sidetone(hal_audio_device_parameter_sidetone_t *handle, hal_audio_control_t device, hal_audio_control_status_t control);

bool hal_audio_device_analog_set_input(hal_audio_device_parameter_t *handle, hal_audio_control_t device, hal_audio_control_status_t control);
bool hal_audio_device_analog_set_output(hal_audio_device_parameter_dac_t *dac_param, hal_audio_control_t device, hal_audio_control_status_t control);
uint32_t hal_audio_device_get_rate(hal_audio_agent_t agent);
bool hal_audio_device_set_amp_output_gpio_id(uint32_t gpio);
bool hal_audio_device_set_gpio_on_delay_timer(uint32_t timer_ms);
bool hal_audio_device_set_amp_delay_timer(uint32_t timer_ms);
bool hal_audio_device_force_off_delay_timer(void);
bool hal_audio_device_set_notice_off_handler(hal_audio_handler_entry handler);
bool hal_audio_device_set_vad_start(hal_audio_vad_start_parameter_t *vad_start);

bool hal_audio_device_setting(hal_audio_device_parameter_t *handle, hal_audio_control_t device, hal_audio_control_status_t control);
bool hal_audio_memory_setting(hal_audio_memory_parameter_t *handle, hal_audio_control_t memory_interface, hal_audio_control_status_t control);

int32_t hal_audio_control_get_src_xppm(hal_audio_src_compensation_parameter_t *src_compensation);
uint32_t hal_audio_control_get_src_input_sample_count(hal_audio_memory_selection_t *memory_sselect);
bool hal_audio_control_set_src_compensation(hal_audio_src_compensation_parameter_t  *src_compensation);
bool hal_audio_src_set_parameters(hal_audio_memory_parameter_t *handle, afe_src_configuration_t *configuration);
bool hal_audio_src_configuration(afe_src_configuration_t *configuration, hal_audio_control_status_t control);
afe_asrc_id_t hal_audio_src_get_id(hal_audio_memory_selection_t memory_select);
bool hal_audio_src_set_start(afe_src_configuration_t *configuration, hal_audio_memory_sync_selection_t sync_select, hal_audio_control_status_t control);

afe_i2s_apll_t hal_audio_i2s_get_apll_by_samplerate(uint32_t samplerate);
bool hal_audio_i2s_set_apll(afe_i2s_apll_t apll_source, hal_audio_control_status_t control);
bool hal_audio_i2s_set_low_jitter(afe_i2s_apll_t apll_source, hal_audio_control_status_t control);
bool hal_audio_i2s_set_mclk(afe_i2s_apll_t apll_source, afe_i2s_id_t i2s_id, uint32_t mclk_divider, bool enable);
bool hal_audio_i2s_set_clk(afe_i2s_id_t i2s_id, bool enable);

uint32_t hal_audio_control_set_sine_generator (hal_audio_sine_generator_parameter_t *generator_handle);

bool hal_audio_memory_change_irq_period(hal_audio_memory_irq_period_parameter_t *handle);
bool hal_audio_memory_set_irq_enable(hal_audio_memory_irq_enable_parameter_t *handle);

void hal_audio_adda_set_enable(bool enable);
void hal_audio_adda_set_global_bias_enable(bool enable);
bool hal_audio_adda_set_ul(hal_audio_agent_t device_agent, hal_audio_ul_iir_t iir_filter, uint32_t samplerate, hal_audio_control_status_t control);
bool hal_audio_adda_set_bias_enable(hal_audio_bias_selection_t bias_select, hal_audio_bias_voltage_t *bias_voltage, bool is_low_power,bool bias1_2_with_LDO0, hal_audio_control_status_t control);

bool hal_audio_device_set_mic_bias(hal_audio_mic_bias_parameter_t *mic_bias);

bool hal_audio_adda_set_dl(hal_audio_agent_t device_agent, uint32_t samplerate, hal_audio_dl_sdm_setting_t sdm_setting, hal_audio_control_status_t control);

bool hal_audio_volume_set_digital_gain_setting(hal_audio_volume_digital_gain_setting_parameter_t *gain_setting);
bool hal_audio_volume_set_digital_gain(hal_audio_volume_digital_gain_parameter_t *digital_gain);
bool hal_audio_volume_set_digital_gain_fade_time_setting(hal_audio_volume_digital_gain_fade_time_setting_parameter_t *gain_fade_tiem_setting);

bool hal_audio_volume_set_analog_input_gain(hal_audio_volume_analog_input_gain_parameter_t *input_gain);
bool hal_audio_volume_set_analog_output_gain(hal_audio_volume_analog_output_gain_parameter_t *output_gain);
bool hal_audio_volume_set_analog_output_mode(hal_audio_volume_analog_output_mode_parameter_t *output_mode);
#ifdef AIR_A2DP_NLE_ENABLE
extern bool hal_audio_dl_set_nle(VOID* DestBuf, U16 CopySize, U32 format, SOURCE_TYPE type);
#endif
#ifdef AIR_AUDIO_LR_OUT_ANALOG_GAIN_OFFSET_ENABLE
bool hal_audio_volume_set_analog_output_offset_gain(hal_audio_volume_analog_output_gain_parameter_t *output_gain);
#endif
bool hal_audio_slave_set_vdma(hal_audio_slave_vdma_parameter_t *vdma_setting);

#ifdef AIR_SIDETONE_ENABLE
void hal_audio_sidetone_timer_callback(HAL_AUDIO_TIMER_HANDLE xTimer);
#endif
void hal_audio_amp_delay_off_timer_callback(HAL_AUDIO_TIMER_HANDLE xTimer);
void hal_audio_vad_delay_timer_callback(HAL_AUDIO_TIMER_HANDLE xTimer);
void hal_audio_vow_timer_callback(HAL_AUDIO_TIMER_HANDLE xTimer);
#if (HAL_AUDIO_VOW_DEBUG)
void hal_audio_vow_degug_timer_callback(HAL_AUDIO_TIMER_HANDLE xTimer);
#endif
hal_audio_performance_mode_t hal_get_adc_performance_mode(afe_analog_select_t analog_select);
void hal_save_adc_performance_mode(afe_analog_select_t analog_select,uint8_t adc_mode);

extern VOID DSP_D2C_BufferCopy(VOID* DestBuf,VOID* SrcBuf,U16 CopySize,VOID* CBufStart,U16 DestCBufSize);

int isPowerOfTwo(unsigned int n);
int findPosition(unsigned int n);

extern void afe_send_amp_status_ccni(bool enable);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functiion Declaration //////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************************
*                                      Audio Control                                       *
********************************************************************************************/
void hal_audio_control_initialize(void)
{
    hal_audio_irq_parameter_t irq_param;
    UNUSED(irq_param);
    afe_control_adda_counter = 0;
    afe_control_special_isr_counter = 0;

    memset(afe_control_bias_counter, 0, sizeof(afe_control_bias_counter));

#ifdef AIR_SIDETONE_ENABLE
    sidetone_control.target_positive_gain = sidetone_control.current_positive_gain = 0;
    sidetone_control.target_negative_gain = sidetone_control.current_negative_gain = HAL_AUDIO_SIDETONE_MUTE_NEGATIVE_VALUE;
    sidetone_control.timer_handle = HAL_AUDIO_TIMER_CREATE("Sidetone_timer", HAL_AUDIO_SIDETONE_RAMP_TIMER_MS, true, hal_audio_sidetone_timer_callback);
#endif

    hal_audio_device_set_amp_output_gpio_id(HAL_AUDIO_AMP_OUTPUT_GPIO);
    amp_control.delay_handle.agent                      = HAL_AUDIO_AGENT_ERROR;
    amp_control.delay_handle.delay_output_off_time_ms   = HAL_AUDIO_DELAY_OUTPUT_OFF_TIME_MS;
    amp_control.delay_handle.delay_gpio_on_time_ms      = HAL_AUDIO_DELAY_GPIO_ON_TIME_MS;
    amp_control.delay_handle.timer_handler  = HAL_AUDIO_TIMER_CREATE("AMP_timer", (amp_control.delay_handle.delay_output_off_time_ms!=0)? amp_control.delay_handle.delay_output_off_time_ms : 10, false, hal_audio_amp_delay_off_timer_callback);

    #if (HAL_AUDIO_VAD_DRIVER)
    vad_control.timer_handle  = HAL_AUDIO_TIMER_CREATE("VAD_timer", HAL_AUDIO_VAD_DELAYON_TIMER_MS, false, hal_audio_vad_delay_timer_callback);
    #endif
    vow_control.timer_handle = HAL_AUDIO_TIMER_CREATE("VOW_timer", HAL_AUDIO_VOW_STABLE_TIMER_MS, true, hal_audio_vow_timer_callback);
    #if (HAL_AUDIO_VOW_DEBUG)
    vow_debug_timer_handle = HAL_AUDIO_TIMER_CREATE("VOW_debug_timer", HAL_AUDIO_VOW_STABLE_TIMER_MS*10, true, hal_audio_vow_degug_timer_callback);
    #endif
#ifdef MTK_ANC_ENABLE
    hal_audio_anc_init();
#endif
    // audio_device_semaphore = xSemaphoreCreateBinary();
    // if(!audio_device_semaphore) {
    //     DSP_MW_LOG_I("fw_amp_init_semaphore create semaphore FAIL \n", 0);
    // } else {
    //     xSemaphoreGive(audio_device_semaphore);
    // }
}


uint32_t hal_audio_control_get_value(hal_audio_get_value_parameter_t *handle, hal_audio_get_value_command_t command)
{
    uint32_t get_value = 0;
    switch (command) {
        case HAL_AUDIO_GET_AUDIO_STATUS:
            get_value = hal_audio_control_get_audio_count();
            break;
        case HAL_AUDIO_GET_CONTROL_COUNT:
            get_value = hal_audio_control_get_agent_count(&(handle->get_control_count));
            break;
        case HAL_AUDIO_GET_MEMORY_INPUT_CURRENT_OFFSET:
        case HAL_AUDIO_GET_MEMORY_OUTPUT_CURRENT_OFFSET:
            get_value = hal_audio_control_get_current_offset(&(handle->get_current_offset), command);
            break;
        case HAL_AUDIO_GET_MEMORY_PLAYEN_MONITOR:
            get_value = hal_memory_get_palyen_monitor(HAL_AUDIO_AGENT_MEMORY_DL1);
            break;
        case HAL_AUDIO_GET_MEMORY_SRC_INPUT_SAMPLE_COUNT:
            get_value = hal_audio_control_get_src_input_sample_count(&(handle->get_src_sample_count));
            break;
        case HAL_AUDIO_GET_MEMORY_INFORMATION:
            get_value = (uint32_t)hal_audio_memory_get_info(&(handle->get_memory_information));
            break;
        case HAL_AUDIO_GET_DEVICE_SAMPLE_RATE:
            get_value = hal_audio_device_get_rate(hal_device_convert_agent(handle->get_device_rate.device_control, handle->get_device_rate.device_interface));
            break;
        case HAL_AUDIO_GET_DEVICE_DAC_COMPENSATION_VALUE:
            if (handle->get_dl_dc_compensation == HAL_AUDIO_ANALOG_OUTPUT_CLASSD) {
                get_value = hal_audio_dl_get_classd_compensation_value();
            } else {
                get_value = hal_audio_dl_get_classab_compensation_value();
            }
            break;
        case HAL_AUDIO_GET_SRC_XPPM:
            get_value = (uint32_t)hal_audio_control_get_src_xppm(&(handle->src_compensation));
            break;
        #ifdef MTK_ANC_ENABLE
        case HAL_AUDIO_GET_ANC_REG:
            get_value = (uint32_t)hal_audio_anc_get_reg(handle);
            break;
        #endif
        #ifdef HWSRC_CLOCK_SKEW
        case HAL_AUDIO_GET_CLOCK_SKEW_ASRC_COMPENSATED_SAMPLE:
            //clock_skew_asrc_get_compensated_sample(&(handle->get_src_clock_skew_cp));
            break;
        #endif
        default:
            HAL_AUDIO_LOG_ERROR("DSP - Error Hal Audio [%s], command:%d !", 2, __FUNCTION__, command);
            break;
    }
    return get_value;
}

hal_audio_status_t hal_audio_control_set_value(hal_audio_set_value_parameter_t *handle, hal_audio_set_value_command_t command)
{
    hal_audio_status_t audio_status = HAL_AUDIO_STATUS_OK;
    switch (command) {
        case HAL_AUDIO_SET_TRIGGER_MEMORY_START:
            hal_audio_memory_sw_trigger(handle->sw_trigger_start.memory_select, handle->sw_trigger_start.enable);
            break;
        case HAL_AUDIO_SET_SRC_INPUT_CURRENT_OFFSET:
        case HAL_AUDIO_SET_SRC_output_CURRENT_OFFSET:
            hal_audio_control_set_current_offset(&(handle->set_current_offset), command);
            break;
        case HAL_AUDIO_SET_IRQ_HANDLER:
            hal_audio_irq_register(&(handle->register_irq_handler));
            break;
        case HAL_AUDIO_SET_SINE_GENERATOR:
            hal_audio_control_set_sine_generator(&(handle->sine_generator));
            break;
        case HAL_AUDIO_SET_SRC_COMPENSATION:
            hal_audio_control_set_src_compensation(&(handle->src_compensation));
            break;
        case HAL_AUDIO_SET_MEMORY_IRQ_PERIOD:
            hal_audio_memory_change_irq_period(&(handle->irq_period));
            break;
        case HAL_AUDIO_SET_MEMORY_IRQ_ENABLE:
            hal_audio_memory_set_irq_enable((&(handle->irq_enable)));
            break;
        case HAL_AUDIO_SET_DEVICE_AMP_OUTPUT_GPIO:
            hal_audio_device_set_amp_output_gpio_id(handle->value);
            break;
        case HAL_AUDIO_SET_DEVICE_HOLD_AMP_OUTPUT_GPIO:
            hal_audio_device_hold_output_gpio((bool)handle->value);
            break;
        case HAL_AUDIO_SET_DEVICE_SET_AMP_OUTPUT_GPIO_STATUS:
            hal_audio_device_set_output_gpio((hal_audio_control_status_t) handle->value, true);
            break;
        case HAL_AUDIO_SET_DEVICE_OUTPUT_GPIO_DELAY_TIMER_MS:
            hal_audio_device_set_gpio_on_delay_timer(handle->value);
            break;
        case HAL_AUDIO_SET_DEVICE_AMP_DELAY_TIMER_MS:
            hal_audio_device_set_amp_delay_timer(handle->value);
            break;
        case HAL_AUDIO_SET_DEVICE_FORCE_OFF:
            hal_audio_device_force_off_delay_timer();
            break;
        case HAL_AUDIO_SET_DEVICE_NOTICE_OFF_HANDLER:
            hal_audio_device_set_notice_off_handler((hal_audio_handler_entry)handle->value);
            break;
        case HAL_AUDIO_SET_DEVICE_MIC_BIAS:
            hal_audio_device_set_mic_bias((hal_audio_mic_bias_parameter_t *)&handle->mic_bias);
            break;
        case HAL_AUDIO_SET_DEVICE_VAD_START:
            hal_audio_device_set_vad_start((hal_audio_vad_start_parameter_t *)&handle->vad_start);
            break;


        case HAL_AUDIO_SET_VOLUME_HW_DIGITAL_SETTING:
            hal_audio_volume_set_digital_gain_setting(&(handle->digital_gain_setting));
            break;
        case HAL_AUDIO_SET_VOLUME_HW_DIGITAL_GAIN:
            hal_audio_volume_set_digital_gain(&(handle->digital_gain));
            break;
        case HAL_AUDIO_SET_VOLUME_HW_DIGITAL_FADE_TIME_SETTING:
            #if (HAL_AUDIO_HWGAIN_SET_FADE_TIME)
            hal_audio_volume_set_digital_gain_fade_time_setting(&(handle->digital_gain_fade_time_setting));
            #endif
            break;
        case HAL_AUDIO_SET_VOLUME_INPUT_ANALOG_GAIN:
            hal_audio_volume_set_analog_input_gain(&(handle->analog_input_gain));
            break;
        case HAL_AUDIO_SET_VOLUME_OUTPUT_ANALOG_GAIN:
            hal_audio_volume_set_analog_output_gain(&(handle->analog_output_gain));
            break;
        case HAL_AUDIO_SET_VOLUME_OUTPUT_ANALOG_SETTING:
            hal_audio_volume_set_analog_output_mode(&(handle->analog_output_mode));
            break;
#ifdef AIR_AUDIO_LR_OUT_ANALOG_GAIN_OFFSET_ENABLE
        case HAL_AUDIO_SET_VOLUME_OUTPUT_ANALOG_GAIN_OFFSET:
            hal_audio_volume_set_analog_output_offset_gain(&(handle->analog_output_gain));
            break;
#endif
        #ifdef MTK_ANC_ENABLE
        case HAL_AUDIO_SET_ANC_REG:
            hal_audio_anc_set_reg(handle);
            break;
        #endif
        case HAL_AUDIO_SET_SLAVE_VDMA:
            hal_audio_slave_set_vdma(&(handle->slave_vdma));
            break;
        case HAL_AUDIO_SET_AUDIO_CLOCK:
            hal_audio_afe_set_enable((bool)handle->value);
            break;

        default:
            audio_status = HAL_AUDIO_STATUS_ERROR;
            HAL_AUDIO_LOG_ERROR("DSP - Error Hal Audio [%s], command:%d !", 2, __FUNCTION__, command);
            break;
    }
    return audio_status;
}


uint32_t hal_audio_control_get_current_offset(hal_audio_current_offset_parameter_t *offset_handle, hal_audio_get_value_command_t command)
{
    hal_audio_agent_t memory_agent = hal_memory_convert_agent(offset_handle->memory_select);
    offset_handle->offset = 0;
    //while(!offset_handle->offset) {
        //TEMP workaround : Get current is 0.
        if ((offset_handle->pure_agent_with_src) ||
            ((memory_agent>=HAL_AUDIO_AGENT_MEMORY_SRC_MIN) && (memory_agent<=HAL_AUDIO_AGENT_MEMORY_SRC_MAX))) {
            afe_asrc_id_t asrc_id = hal_audio_src_get_id(offset_handle->memory_select);
            if (command == HAL_AUDIO_GET_MEMORY_INPUT_CURRENT_OFFSET) {
                offset_handle->offset = hal_src_get_input_read_offset(asrc_id);
                offset_handle->base_address = hal_src_get_input_base_address(asrc_id);
            } else {
                offset_handle->offset = hal_src_get_output_write_offset(asrc_id);
                offset_handle->base_address = hal_src_get_output_base_address(asrc_id);
            }

        } else {
            offset_handle->offset = hal_memory_get_offset(memory_agent);
            offset_handle->base_address = hal_memory_get_address(memory_agent);
        }
        if(!offset_handle->offset) {
            HAL_AUDIO_LOG_WARNING("DSP - Warning Hal Audio Get Current Offset is ZERO @@", 0);
        }
    //}
    return offset_handle->offset;
}


uint32_t hal_audio_control_set_current_offset(hal_audio_current_offset_parameter_t *offset_handle, hal_audio_set_value_command_t command)
{
    hal_audio_agent_t memory_agent = hal_memory_convert_agent(offset_handle->memory_select);

    if ((offset_handle->pure_agent_with_src) ||
        ((memory_agent>=HAL_AUDIO_AGENT_MEMORY_SRC_MIN) && (memory_agent<=HAL_AUDIO_AGENT_MEMORY_SRC_MAX))) {
        afe_asrc_id_t asrc_id = hal_audio_src_get_id(offset_handle->memory_select);
        if (command == HAL_AUDIO_SET_SRC_INPUT_CURRENT_OFFSET) {
            hal_src_set_input_write_offset(asrc_id, offset_handle->offset);
        } else {
            hal_src_set_output_read_offset(asrc_id, offset_handle->offset);
        }

    } else {

    }
    return offset_handle->offset;
}

uint32_t hal_audio_control_get_agent_count(hal_audio_control_select_parameter_t *audio_select)
{
    hal_audio_agent_t agent;
    if (audio_select->audio_control == HAL_AUDIO_CONTROL_MEMORY_INTERFACE) {
        agent = hal_memory_convert_agent(audio_select->audio_port.memory_select);
    } else {
        agent = hal_device_convert_agent(audio_select->audio_control, audio_select->audio_port.device_interface);
    }
    return hal_audio_agent_user_count[agent];
}


uint32_t hal_audio_control_get_audio_count(void)
{
    //Except for  ANC, VAD
    return (uint32_t)(hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_INPUT]+hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_OUTPUT]);
}


/*******************************************************************************************
*                                       Device agent                                       *
********************************************************************************************/
//HAL_AUDIO_SEMAPHO_HANDLE audio_device_semaphore;
int hal_audio_device_semaphore_take(HAL_AUDIO_SEMAPHO_HANDLE xSemaphore,uint32_t xBlockTime){
    BaseType_t ret;
    if (HAL_NVIC_QUERY_EXCEPTION_NUMBER > HAL_NVIC_NOT_EXCEPTION ) {
        BaseType_t xHigherPriorityTaskWoken;
        ret = xSemaphoreTakeFromISR(xSemaphore,&xHigherPriorityTaskWoken );
        if(ret == pdFALSE){
            HAL_AUDIO_LOG_INFO("cannot take semaphore, will yield the irq",0);
        }
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }else{
        ret = xSemaphoreTake(xSemaphore, xBlockTime);
        if(ret == pdFALSE){
            HAL_AUDIO_LOG_INFO("cannot take semaphore, will yield the task",0);
        }
        portYIELD();
    }
    return ret;
}

int hal_audio_device_semaphore_give(HAL_AUDIO_SEMAPHO_HANDLE xSemaphore){
    BaseType_t ret;
    if (HAL_NVIC_QUERY_EXCEPTION_NUMBER > HAL_NVIC_NOT_EXCEPTION ) {
        BaseType_t xHigherPriorityTaskWoken;
        ret = xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken );
        if(ret == pdFALSE){
            HAL_AUDIO_LOG_INFO("cannot give semaphore, will yield the irq",0);
        }
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }else{
        ret = xSemaphoreGive(xSemaphore);
        if(ret == pdFALSE){
            HAL_AUDIO_LOG_INFO("cannot give semaphore, will yield the task",0);
        }
        portYIELD();
    }
    return ret;
}
UBaseType_t uxSavedInterruptState;
void hal_audio_enter_criticl_section(){
    if (HAL_NVIC_QUERY_EXCEPTION_NUMBER > HAL_NVIC_NOT_EXCEPTION ) {
        uxSavedInterruptState = portSET_INTERRUPT_MASK_FROM_ISR();
    } else { vTaskEnterCritical(); }
}
void hal_audio_exit_criticl_section(){
    if (HAL_NVIC_QUERY_EXCEPTION_NUMBER > HAL_NVIC_NOT_EXCEPTION ) {
        portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedInterruptState);
    } else { vTaskExitCritical();  }
}
extern hal_audio_device_parameter_vow_t *dsp_vow;
static hal_audio_irq_entry afe_irq_vow_snr_function;


bool hal_audio_device_set_agent(hal_audio_device_parameter_t *handle, hal_audio_control_t device, hal_audio_control_status_t control)
{
    hal_audio_agent_t agent = hal_device_convert_agent(device, handle->common.device_interface);
    HAL_AUDIO_LOG_INFO("hal_audio_device_set_agent %d on/off %d user_conut %d",3,device,control,hal_audio_agent_user_count[agent]);
    int32_t device_number_for_status = agent-HAL_AUDIO_AGENT_DEVICE_MIN;
    int32_t using_count = (int32_t)hal_audio_device_get_using_count(handle, device);
    hal_audio_device_status_t *device_status = ((device_number_for_status>=0) && (device_number_for_status<HAL_AUDIO_AGENT_DEVICE_NUMBERS))
                                                 ? &hal_audio_device_status[device_number_for_status]
                                                 : (hal_audio_device_status_t *)&device_number_for_status;

    if (control == HAL_AUDIO_CONTROL_ON) {
        if (!hal_audio_device_set_delay_timer(handle, device, control)) {
            HAL_AUDIO_ENTER_CRITICAL();
            hal_audio_agent_user_count[agent]+=using_count;
            *device_status = HAL_AUDIO_DEVICE_STATUS_OPEN;
            if (hal_audio_agent_user_count[agent] == using_count) {
                HAL_AUDIO_EXIT_CRITICAL();
                HAL_AUDIO_LOG_INFO("afe_set on agnet %d device %d\r\n",2,agent,device);
                hal_audio_afe_set_enable(true);
                hal_audio_device_setting(handle, device, control);
            } else {
                HAL_AUDIO_EXIT_CRITICAL();
                //Distinguish L&R
                hal_audio_device_distinguish_channel(handle, device, control);
            }
        } else {
            // Wait delay timer
        }
    } else {
        HAL_AUDIO_ENTER_CRITICAL();
        hal_audio_agent_user_count[agent]-=using_count;
        if (hal_audio_agent_user_count[agent]>0) {
            HAL_AUDIO_EXIT_CRITICAL();
            //Distinguish L&R
            hal_audio_device_distinguish_channel(handle, device, control);
        } else if (hal_audio_agent_user_count[agent]==0) {
            if ((*device_status == HAL_AUDIO_DEVICE_STATUS_PENDING) ||
                (!hal_audio_device_set_delay_timer(handle, device, control))) {
                //No delay timer
                HAL_AUDIO_EXIT_CRITICAL();
                *device_status = HAL_AUDIO_DEVICE_STATUS_STOP;
                hal_audio_device_setting(handle, device, control);
                HAL_AUDIO_LOG_INFO("afe_set off agnet %d device %d\r\n",2,agent,device);
                hal_audio_afe_set_enable(false);
            } else {
                //Delay
                hal_audio_agent_user_count[agent]+=using_count;
                *device_status = HAL_AUDIO_DEVICE_STATUS_PENDING;
                HAL_AUDIO_EXIT_CRITICAL();
            }
        } else {
            hal_audio_agent_user_count[agent] = 0;
            HAL_AUDIO_EXIT_CRITICAL();
            HAL_AUDIO_LOG_ERROR("DSP - Error Hal Audio Agent Wrong user counter %d !", 1, agent);
        }
    }
    #if (HAL_AUDIO_KEEP_ADC_HIGHER_PERFORMANCE_MODE)
        if(dsp_vow &&(device!=HAL_AUDIO_CONTROL_DEVICE_VOW)){//force vow to switch phase 0 to phase 1
            afe_analog_select_t analog_select = AFE_ANALOG_ADC0;
            if (handle->analog_mic.mic_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2) {

                analog_select = AFE_ANALOG_ADC1;
            } else if (handle->analog_mic.mic_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3) {
                analog_select = AFE_ANALOG_ADC2;
            }
            HAL_AUDIO_LOG_INFO("VOw eixt adc count %d,if %d,cur if",3,afe_analog_control[analog_select].counter,dsp_vow->mic_interface,handle->analog_mic.mic_interface);
            if(dsp_vow->mic_interface == handle->analog_mic.mic_interface && afe_analog_control[analog_select].counter){
                //DTM_enqueue(DTM_EVENT_ID_VOW_DISABLE, 0, false);
                if (afe_irq_vow_snr_function) {
                    HAL_AUDIO_LOG_INFO("force VOW to switch phase 0 to phase 1 %d",1,control);
                    afe_irq_vow_snr_function();
                }
            }
        }
    #endif
    return false;
}

bool hal_audio_device_setting(hal_audio_device_parameter_t *handle, hal_audio_control_t device, hal_audio_control_status_t control)
{
    switch (device) {
        case HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_L:
        case HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_R:
        case HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL:
            hal_audio_device_set_analog_mic(&handle->analog_mic, device, control);
            break;
        case HAL_AUDIO_CONTROL_DEVICE_LINE_IN_L:
        case HAL_AUDIO_CONTROL_DEVICE_LINE_IN_R:
        case HAL_AUDIO_CONTROL_DEVICE_LINE_IN_DUAL:
            hal_audio_device_set_linein(&handle->linein, device, control);
            break;
        case HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_L:
        case HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_R:
        case HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL:
            hal_audio_device_set_digital_mic(&handle->digital_mic, device, control);
            break;
        case HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_L:
        case HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_R:
        case HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_DUAL:
            hal_audio_device_set_dac(&handle->dac, device, control);
            break;
        case HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER:
        case HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_L:
        case HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_R:
            hal_audio_device_set_i2s_master(&handle->i2s_master, device, control);
            break;
        case HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE:
            hal_audio_device_set_i2s_slave(&handle->i2s_slave, device, control);
            break;
        case HAL_AUDIO_CONTROL_DEVICE_SPDIF:
            hal_audio_device_set_spdif(&handle->spdif, device, control);
            break;
        case HAL_AUDIO_CONTROL_DEVICE_LOOPBACK:
            hal_audio_device_set_ul_loopback(&handle->loopback, device, control);
            break;
        #ifdef MTK_ANC_ENABLE
        case HAL_AUDIO_CONTROL_DEVICE_ANC:
            hal_audio_device_set_anc(handle, device, control);
            break;
        #endif
        case HAL_AUDIO_CONTROL_DEVICE_VAD:
            hal_audio_device_set_vad(&handle->vad, device, control);
            break;
        case HAL_AUDIO_CONTROL_DEVICE_VOW:
            hal_audio_device_set_vow(&handle->vow, device, control);
            break;
        #ifdef AIR_SIDETONE_ENABLE
        case HAL_AUDIO_CONTROL_DEVICE_SIDETONE:
            hal_audio_device_set_sidetone(&handle->sidetone, device, control);
            break;
        #endif
        default:
            break;
    }

    HAL_AUDIO_LOG_INFO("DSP - Hal Audio device:0x%x, Off/On:%d", 2, device, control);
    return hal_audio_device_set_inout_cnt(device, control);
}


bool hal_audio_device_set_inout_cnt(hal_audio_control_t device, hal_audio_control_status_t control)
{
    if (device&(HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL |
                HAL_AUDIO_CONTROL_DEVICE_LINE_IN_DUAL |
                HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL |
                HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER |
                HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_L |
                HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_R |
                HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE)) {
        if (control == HAL_AUDIO_CONTROL_ON) {
            hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_INPUT]++;
            if (hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_INPUT]==1) {

            }
        } else {
            if (hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_INPUT]==1) {

            } else if (hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_INPUT]<0) {
                hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_INPUT] = 0;
            }
            hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_INPUT]--;
        }
    }

    if (device&(HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_DUAL |
                HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER |
                HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_L |
                HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_R |
                HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE |
                HAL_AUDIO_CONTROL_DEVICE_SPDIF)) {
        if (control == HAL_AUDIO_CONTROL_ON) {
            hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_OUTPUT]++;
            if (hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_OUTPUT]==1) {

            }
        } else {
            if (hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_OUTPUT]==1) {

            } else if (hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_OUTPUT]<0) {
                hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_OUTPUT] = 0;
            }
            hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_OUTPUT]--;
        }
    }

    if (!hal_audio_control_get_audio_count()) {
        if (amp_control.notice_off_handler) {
            amp_control.notice_off_handler();
        }
    }

    return false;
}

bool hal_audio_device_update_output_gpio(void)
{
    bool status = false;
    HAL_AUDIO_ENTER_CRITICAL();
    if (amp_control.output_gpio_status || amp_control.is_hold_output_gpio) {
        status = true;
        HAL_AUDIO_GPIO_SET_STATUS(amp_control.output_gpio, HAL_AUDIO_GPIO_STATUS_HIGH);
    } else {
        HAL_AUDIO_GPIO_SET_STATUS(amp_control.output_gpio, HAL_AUDIO_GPIO_STATUS_LOW);
    }
    HAL_AUDIO_EXIT_CRITICAL();
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio AMP gpio:%d, status:%d", 2, amp_control.output_gpio, status);
    return status;
}


bool hal_audio_device_hold_output_gpio(bool is_hold)
{
    if (amp_control.output_gpio != HAL_AUDIO_AMP_OUTPUT_GPIO_DISABLE) {
        amp_control.is_hold_output_gpio = is_hold;
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio AMP hold GPIO output:%d", 1, is_hold);
        hal_audio_device_update_output_gpio();
    }
    return amp_control.is_hold_output_gpio;
}

bool hal_audio_device_set_output_gpio(hal_audio_control_status_t control, bool is_time_up)
{
    if (amp_control.output_gpio != HAL_AUDIO_AMP_OUTPUT_GPIO_DISABLE) {
        if (control == HAL_AUDIO_CONTROL_ON) {
            if ((amp_control.delay_handle.delay_gpio_on_time_ms) && (is_time_up==false)) {
                HAL_AUDIO_TIMER_START(amp_control.delay_handle.timer_handler, amp_control.delay_handle.delay_gpio_on_time_ms);
                return true;
            } else {
                amp_control.output_gpio_status = true;
            }
        } else {
            amp_control.output_gpio_status = false;
        }
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio AMP gpio:%d, control:%d", 2, amp_control.output_gpio, control);
        hal_audio_device_update_output_gpio();
    }
    return false;
}

U32 hal_audio_device_get_gpio(void)
{
    return amp_control.output_gpio;
}

bool hal_audio_device_set_delay_timer(hal_audio_device_parameter_t *handle, hal_audio_control_t device, hal_audio_control_status_t control)
{
    bool with_timer = false;
    #if 0
    if (device&HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_DUAL) {
        if (control == HAL_AUDIO_CONTROL_OFF) {
            if (amp_control.delay_handle.delay_output_off_time_ms) {
                //set timer
                memcpy(&amp_control.delay_handle.device_parameter, handle, sizeof(hal_audio_device_parameter_t));
                HAL_AUDIO_TIMER_START(amp_control.delay_handle.timer_handler, amp_control.delay_handle.delay_output_off_time_ms);
            } else {
                hal_audio_device_set_agent(handle, device, control);
            }
        } else {
            //cancel timer
            HAL_AUDIO_TIMER_STOP(amp_control.delay_handle.timer_handler);
        }
        with_timer = true;
    } else {

    }
    #else
    if (device&HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_DUAL) {
        //output device
        if (control == HAL_AUDIO_CONTROL_OFF) {
            //if ((hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_OUTPUT]-hal_audio_agent_user_count[HAL_AUDIO_AGENT_MEMORY_DL12])==1) {
            if (hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_ADDA_DL1]==0) {
                if (amp_control.delay_handle.delay_output_off_time_ms) {
                    //Set timer
                    if (amp_control.delay_handle.agent != HAL_AUDIO_AGENT_ERROR) {
                        HAL_AUDIO_LOG_WARNING("DSP - Warning Hal Audio another output device close while timer started %d @@", 1, amp_control.delay_handle.agent);
                        hal_audio_amp_delay_off_timer_callback(NULL);
                    }
                    memcpy(&amp_control.delay_handle.device_parameter, handle, sizeof(hal_audio_device_parameter_t));
                    amp_control.delay_handle.agent = hal_device_convert_agent(device, handle->common.device_interface);
                    HAL_AUDIO_TIMER_STOP(amp_control.delay_handle.timer_handler);
                    HAL_AUDIO_TIMER_START(amp_control.delay_handle.timer_handler, amp_control.delay_handle.delay_output_off_time_ms);
                    HAL_AUDIO_LOG_INFO("DSP - Hal Audio output delay off timer start %d(ms)", 1, amp_control.delay_handle.delay_output_off_time_ms);
                    with_timer = true;
                } else {
                    afe_send_amp_status_ccni(false);
                }
                hal_audio_device_set_output_gpio(control, false);
            }
        } else {
            //if ((hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_OUTPUT]-hal_audio_agent_user_count[HAL_AUDIO_AGENT_MEMORY_DL12])==0) {
            if (hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_ADDA_DL1]==0) {
                //First output device except for datadump
                hal_audio_device_set_output_gpio(control, false);
            }
            if (amp_control.delay_handle.agent != HAL_AUDIO_AGENT_ERROR) {
                //Cancel timer
                if (device != amp_control.delay_handle.device_parameter.common.audio_device) {
                    //Different device
                    HAL_AUDIO_LOG_WARNING("DSP - Warning different device set off timer New:%d ,Old:%d @@", 2, device ,amp_control.delay_handle.device_parameter.common.audio_device);

                } else {
                    //Same device
                    HAL_AUDIO_TIMER_STOP(amp_control.delay_handle.timer_handler);
                    HAL_AUDIO_LOG_INFO("DSP - Hal Audio output delay off timer cancel %d", 1, amp_control.delay_handle.agent);
                    hal_audio_device_status[amp_control.delay_handle.agent-HAL_AUDIO_AGENT_DEVICE_MIN] = HAL_AUDIO_DEVICE_STATUS_OPEN;
                    amp_control.delay_handle.agent = HAL_AUDIO_AGENT_ERROR;
                    #if (HAL_AUDIO_CHANGE_OUTPUT_RATE)
                    hal_audio_device_change_rate(handle, device, control);
                    #endif
                    with_timer = true;
                }
                hal_audio_device_set_output_gpio(control, false);
            }
            afe_send_amp_status_ccni(true);
        }

    }
    #endif



    return with_timer;
}

uint32_t hal_audio_device_get_using_count(hal_audio_device_parameter_t *handle, hal_audio_control_t device)
{
    uint32_t using_count = 1;
#ifdef MTK_ANC_ENABLE
    if(device&HAL_AUDIO_CONTROL_DEVICE_ANC) {
        using_count = hal_audio_anc_get_using_count(handle);
    }
#else
    UNUSED(handle);
    UNUSED(device);
#endif
    return using_count;
}
#if (FEA_SUPP_DSP_VOW)
extern hal_audio_device_parameter_vow_t *dsp_vow;
#endif

bool hal_audio_device_distinguish_channel(hal_audio_device_parameter_t *handle, hal_audio_control_t device, hal_audio_control_status_t control)
{
    UNUSED(handle);
    if (device&HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL) {
        #if (HAL_AUDIO_KEEP_ADC_HIGHER_PERFORMANCE_MODE)
        //If the new ADC performance mode is higher than the current ADC performance mode, it will change the ADC performance mode.
        afe_analog_select_t analog_select = AFE_ANALOG_ADC0;
        hal_audio_device_parameter_adc_t *adc_parameter;
        afe_analog_control_t analog_control = 0;
        adc_parameter = &handle->analog_mic.adc_parameter;
        if (handle->analog_mic.mic_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2) {

            analog_select = AFE_ANALOG_ADC1;
        } else if (handle->analog_mic.mic_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3) {
            analog_select = AFE_ANALOG_ADC2;
        }
        if((adc_parameter->performance < hal_get_adc_performance_mode(analog_select)) && (afe_analog_control[analog_select].counter)){
            if(afe_analog_control[analog_select].channel_counter.channel_l && afe_analog_control[analog_select].channel_counter.channel_r){
                 analog_control |= AFE_ANALOG_COMMON|AFE_ANALOG_L_CH|AFE_ANALOG_R_CH;
            }else if( afe_analog_control[analog_select].channel_counter.channel_l){
                analog_control |= AFE_ANALOG_COMMON|AFE_ANALOG_L_CH;
            }else if( afe_analog_control[analog_select].channel_counter.channel_r){
                analog_control |= AFE_ANALOG_COMMON|AFE_ANALOG_R_CH;
            }

            if(analog_select == AFE_ANALOG_ADC0) {
                HAL_AUDIO_LOG_INFO("switch adc01 performance mode,analog_control %d",1,analog_control);
                hal_audio_ana_set_adc0_enable(adc_parameter, analog_control, false);
                hal_audio_ana_set_adc0_enable(adc_parameter, analog_control, true);
            }
            if(analog_select == AFE_ANALOG_ADC1) {
                HAL_AUDIO_LOG_INFO("switch adc23 performance mode,analog_control %d",1,analog_control);
                hal_audio_ana_set_adc23_enable(adc_parameter, analog_control, false);
                hal_audio_ana_set_adc23_enable(adc_parameter, analog_control, true);

            }
            if(analog_select == AFE_ANALOG_ADC2) {
                HAL_AUDIO_LOG_INFO("switch adc45 performance mode,analog_control %d",1,analog_control);
                hal_audio_ana_set_adc45_enable(adc_parameter, analog_control, false);
                hal_audio_ana_set_adc45_enable(adc_parameter, analog_control, true);

            }

        }
        #endif
        //distinguish_channel ADC
        hal_audio_device_parameter_analog_mic_t *handle_mic = (hal_audio_device_parameter_analog_mic_t *)handle;
        if (!handle_mic->with_external_bias) {
            hal_audio_adda_set_bias_enable(handle_mic->bias_select, (hal_audio_bias_voltage_t *)&(handle_mic->bias_voltage), handle_mic->with_bias_lowpower,handle_mic->bias1_2_with_LDO0, control);
        }
        return hal_audio_device_analog_set_input(handle, device, control);
#if 0
    } else if (device&HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        afe_i2s_id_t i2s_id = hal_i2s_convert_id(device, handle->i2s_master.i2s_interface);
        hal_i2s_slave_set_configuration(handle, i2s_id);
#endif
#ifdef MTK_ANC_ENABLE
    } else if (device&HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_DUAL){
#if 0
        HAL_AUDIO_LOG_INFO("change DL rate check: DL1_cnt(%d), ANC_cnt(%d), handle_rate(%d), DL1_rate(%d), with_force_change_rate(%d)"
            , 5, hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_ADDA_DL1], hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_ANC]
            , handle->dac.rate, afe_samplerate_get_dl_samplerate(), handle->dac.with_force_change_rate);
#endif
        uint32_t dl_require_rate = 0;
        bool is_force_change_rate = false;

        //change DL rate while only ANC running
        if (control == HAL_AUDIO_CONTROL_ON) {
            if (((hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_ADDA_DL1] - hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_ANC]) == 1) &&
                (handle->dac.rate != afe_samplerate_get_dl_samplerate()) &&
                (handle->dac.with_force_change_rate)) {
                dl_require_rate = handle->dac.rate;
                is_force_change_rate= true;
            }
        } else if (control == HAL_AUDIO_CONTROL_OFF) {
            #if !defined(AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ) && !defined(AIR_FIXED_SUB_DL_HIGH_RES_ENABLE)
            dl_require_rate = hal_audio_device_get_rate(HAL_AUDIO_AGENT_DEVICE_ADDA_DL1);
            if ((dl_require_rate > 48000) &&
                (hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_ANC]) &&
                ((hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_ADDA_DL1] - hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_ANC]) == 0)) {
                // Change to 48k
                dl_require_rate = 48000;
                is_force_change_rate= true;
            }
            #endif
        }

        if (is_force_change_rate) {
            HAL_AUDIO_LOG_INFO("DSP - Hal Audio Device %d, distinguish_channel force changing rate :%d", 2, device, dl_require_rate);
            //Ramp down
            afe_volume_digital_set_mute(AFE_HW_DIGITAL_GAIN1, AFE_VOLUME_MUTE_CHANGE_DL_RATE, true);
            afe_volume_digital_set_mute(AFE_HW_DIGITAL_GAIN2, AFE_VOLUME_MUTE_CHANGE_DL_RATE, true);
            afe_volume_digital_set_mute(AFE_HW_DIGITAL_GAIN3, AFE_VOLUME_MUTE_CHANGE_DL_RATE, true);

            hal_audio_anc_set_change_dl_rate(dl_require_rate);
        }

        #if 0
        return hal_audio_device_analog_set_output(handle, device, control);
        #endif
    } else if (device&HAL_AUDIO_CONTROL_DEVICE_ANC){
        if (control == HAL_AUDIO_CONTROL_ON) {
            hal_audio_device_set_anc(handle, HAL_AUDIO_CONTROL_DEVICE_ANC, HAL_AUDIO_CONTROL_ON);
        } else {
            hal_audio_device_set_anc(handle, HAL_AUDIO_CONTROL_DEVICE_ANC, HAL_AUDIO_CONTROL_OFF);
        }
#endif
    }
    return false;
}
bool hal_audio_device_change_rate(hal_audio_device_parameter_t *handle, hal_audio_control_t device, hal_audio_control_status_t control)
{
    UNUSED(control);
    uint32_t device_rate;
    device_rate = hal_audio_device_get_rate(hal_device_convert_agent(device, handle->common.device_interface));
    if (device_rate != handle->common.rate) {
        if (device&HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_DUAL) {
            hal_audio_adda_set_dl(HAL_AUDIO_AGENT_DEVICE_ADDA_DL1, handle->dac.rate, (hal_audio_control_status_t)handle->dac.dl_sdm_setting, HAL_AUDIO_CONTROL_OFF);
            hal_audio_adda_set_dl(HAL_AUDIO_AGENT_DEVICE_ADDA_DL1, handle->dac.rate, (hal_audio_control_status_t)handle->dac.dl_sdm_setting, HAL_AUDIO_CONTROL_ON);
        } else {
            hal_audio_device_setting(handle, device, HAL_AUDIO_CONTROL_OFF);
            hal_audio_device_setting(handle, device, HAL_AUDIO_CONTROL_ON);
        }

        HAL_AUDIO_LOG_INFO("DSP - Hal Audio Device:%d, changing rate :%d", 2, device, handle->common.rate);
    }
    return false;
}



bool hal_audio_device_set_dac(hal_audio_device_parameter_dac_t *handle, hal_audio_control_t device, hal_audio_control_status_t control)
{
    if (control == HAL_AUDIO_CONTROL_ON) {
        hal_audio_adda_set_global_bias_enable(true);

        hal_audio_device_analog_set_output(handle, device, HAL_AUDIO_CONTROL_ON);
        hal_audio_dl_set_inverse(handle->with_phase_inverse);
        hal_audio_adda_set_dl(HAL_AUDIO_AGENT_DEVICE_ADDA_DL1, handle->rate, (hal_audio_dl_sdm_setting_t)handle->dl_sdm_setting, HAL_AUDIO_CONTROL_ON);
    } else {
        hal_audio_device_analog_set_output(handle, device, HAL_AUDIO_CONTROL_OFF);

        hal_audio_adda_set_dl(HAL_AUDIO_AGENT_DEVICE_ADDA_DL1, handle->rate, (hal_audio_dl_sdm_setting_t)handle->dl_sdm_setting, HAL_AUDIO_CONTROL_OFF);

        hal_audio_adda_set_global_bias_enable(false);
    }

   return false;
}

bool hal_audio_device_set_linein(hal_audio_device_parameter_linein_t *handle, hal_audio_control_t device, hal_audio_control_status_t control)
{
    hal_audio_agent_t device_agent = HAL_AUDIO_AGENT_DEVICE_ADDA_UL1;
    if (control == HAL_AUDIO_CONTROL_ON) {
        hal_audio_adda_set_bias_enable(handle->bias_select, (hal_audio_bias_voltage_t *)&(handle->bias_voltage), false,false, control);
        hal_audio_adda_set_global_bias_enable(control);
        hal_audio_device_analog_set_input((hal_audio_device_parameter_t *)handle, device, control);
        hal_audio_ul_set_inverse(device_agent, handle->with_phase_inverse);
        hal_audio_adda_set_ul(device_agent, handle->iir_filter, handle->rate, control);
    } else {
        hal_audio_adda_set_ul(device_agent, handle->iir_filter, handle->rate, control);
        hal_audio_device_analog_set_input((hal_audio_device_parameter_t *)handle, device, control);
        hal_audio_adda_set_global_bias_enable(control);
        hal_audio_adda_set_bias_enable(handle->bias_select, (hal_audio_bias_voltage_t *)&(handle->bias_voltage), false,false, control);
    }
    return false;
}

bool hal_audio_device_set_ul_loopback(hal_audio_device_parameter_loopback_t *handle, hal_audio_control_t device, hal_audio_control_status_t control)
{
    //Loopback from downlink SDM
    hal_audio_agent_t device_agent = hal_device_convert_agent(device, handle->ul_interface);
    if (control == HAL_AUDIO_CONTROL_ON) {
        if (handle->loopback_setting == AFE_AUDIO_UL_LOOPBACK_FROM_DL) {
            hal_audio_ul_set_da_loopback_enable(device_agent, true);
        } else if (device_agent == HAL_AUDIO_AGENT_DEVICE_ADDA_UL4) {
            hal_audio_ul4_set_loopback(handle->loopback_setting, true);
        }
        hal_audio_adda_set_ul(device_agent, handle->iir_filter, handle->rate, control);
    } else {
        hal_audio_adda_set_ul(device_agent, handle->iir_filter, handle->rate, control);
        if (handle->loopback_setting == AFE_AUDIO_UL_LOOPBACK_FROM_DL) {
            hal_audio_ul_set_da_loopback_enable(device_agent, false);
        } else if (device_agent == HAL_AUDIO_AGENT_DEVICE_ADDA_UL4) {
            hal_audio_ul4_set_loopback(handle->loopback_setting, false);
        }
    }
    return false;
}

bool hal_audio_device_set_analog_mic(hal_audio_device_parameter_analog_mic_t *handle, hal_audio_control_t device, hal_audio_control_status_t control)
{
    hal_audio_agent_t device_agent = hal_device_convert_agent(device, handle->mic_interface);
    if (control == HAL_AUDIO_CONTROL_ON) {
        if (!handle->with_external_bias) {
            hal_audio_adda_set_bias_enable(handle->bias_select, (hal_audio_bias_voltage_t *)&(handle->bias_voltage), handle->with_bias_lowpower,handle->bias1_2_with_LDO0, control);
        }
        hal_audio_device_analog_set_input((hal_audio_device_parameter_t *)handle, device, control);
        hal_audio_ul_set_inverse(device_agent, handle->with_phase_inverse);
        hal_audio_adda_set_ul(device_agent, handle->iir_filter, handle->rate, control);
    } else {
        hal_audio_adda_set_ul(device_agent, handle->iir_filter, handle->rate, control);
        hal_audio_device_analog_set_input((hal_audio_device_parameter_t *)handle, device, control);
        if (!handle->with_external_bias) {
            hal_audio_adda_set_bias_enable(handle->bias_select, (hal_audio_bias_voltage_t *)&(handle->bias_voltage), handle->with_bias_lowpower,handle->bias1_2_with_LDO0, control);
        }
    }
    return false;
}

bool hal_audio_device_set_digital_mic(hal_audio_device_parameter_digital_mic_t *handle, hal_audio_control_t device, hal_audio_control_status_t control)
{
    hal_audio_agent_t device_agent = hal_device_convert_agent(device, handle->mic_interface);
    if (control == HAL_AUDIO_CONTROL_ON) {
        if (!handle->with_external_bias) {
            hal_audio_adda_set_bias_enable(handle->bias_select, (hal_audio_bias_voltage_t *)&(handle->bias_voltage), handle->with_bias_lowpower,handle->bias1_2_with_LDO0, control);
            hal_audio_ul_set_dmic_bias(device_agent, true);
        }

        hal_audio_ul_set_dmic_clock(device_agent, handle->dmic_clock_rate);
        hal_audio_ana_set_dmic_enable(handle->dmic_selection, true);
        hal_audio_ul_set_dmic_phase(device_agent, 3, 7);
        hal_audio_ul_set_dmic_selection (device_agent, handle->dmic_selection);
        hal_audio_ul_set_dmic_enable(device_agent, true);

        hal_audio_adda_set_ul(device_agent, handle->iir_filter, handle->rate, control);
    } else {
        hal_audio_adda_set_ul(device_agent, handle->iir_filter, handle->rate, control);

        hal_audio_ul_set_dmic_enable(device_agent, false);
        hal_audio_ana_set_dmic_enable(handle->dmic_selection, false);

        if (!handle->with_external_bias) {
            hal_audio_adda_set_bias_enable(handle->bias_select, (hal_audio_bias_voltage_t *)&(handle->bias_voltage), handle->with_bias_lowpower,handle->bias1_2_with_LDO0, control);
            hal_audio_ul_set_dmic_bias(device_agent, false);
        }
    }
    return false;
}

bool hal_audio_device_set_i2s_slave(hal_audio_device_parameter_i2s_slave_t *handle, hal_audio_control_t device, hal_audio_control_status_t control)
{
    uint32_t  addr_offset;
    afe_i2s_id_t i2s_id = hal_i2s_convert_id(device, handle->i2s_interface);
    addr_offset = i2s_id * 0x10000;

    if (control == HAL_AUDIO_CONTROL_ON) {
        HAL_AUDIO_LOG_INFO("enter i2s slave diver configi2s_id:%d",1,i2s_id);
        #ifdef AIR_DUAL_CHIP_I2S_ENABLE   //for dual chip project
        hal_i2s_slave_gpio_init(i2s_id);
        #endif
#if defined(AIR_HWSRC_RX_TRACKING_ENABLE) || defined(AIR_HWSRC_TX_TRACKING_ENABLE)
        AFE_SET_REG(I2S_SHARE_CK_CONTROL + addr_offset, i2s_id<<I2S_SHARE_CLOCK_INDEX, 0xFFFFFFFF);
#endif
        hal_audio_clock_enable_i2s_slave_hclk(true);
        hal_i2s_slave_set_power(i2s_id, true);
#ifdef MTK_TDM_ENABLE
        if ((handle->memory_select == HAL_AUDIO_MEMORY_DL_SLAVE_DMA) || (handle->memory_select == HAL_AUDIO_MEMORY_UL_SLAVE_DMA)
         || (handle->memory_select == HAL_AUDIO_MEMORY_DL_SLAVE_TDM) || (handle->memory_select == HAL_AUDIO_MEMORY_UL_SLAVE_TDM)) {
#else
        #ifdef AIR_I2S_SLAVE_ENABLE
        if ((handle->memory_select == HAL_AUDIO_MEMORY_DL_SLAVE_DMA) || (handle->memory_select == HAL_AUDIO_MEMORY_UL_SLAVE_DMA) || (handle->memory_select == HAL_AUDIO_MEMORY_UL_VUL1) || (handle->memory_select == HAL_AUDIO_MEMORY_UL_VUL2))
        {
        #else
        if ((handle->memory_select == HAL_AUDIO_MEMORY_DL_SLAVE_DMA) || (handle->memory_select == HAL_AUDIO_MEMORY_UL_SLAVE_DMA))
        {
        #endif
#endif
            hal_i2s_slave_set_clock(); //need check
            if (i2s_id == AFE_I2S0) {
                hal_i2s_slave_set_share_fifo();
                hal_i2s_slave_set_power(AFE_I2S1, true);//Turn on the clocks for TDM1 to enable the FIFOes.
            }
        }
        hal_i2s_slave_set_configuration(handle, i2s_id);
        hal_i2s_slave_set_enable(i2s_id, true);
#ifdef MTK_TDM_ENABLE
        hal_i2s_slave_set_tdm_status(handle, i2s_id);
#endif
    } else {
        hal_i2s_slave_set_enable(i2s_id, false);
        hal_i2s_slave_set_power(i2s_id, false);
        hal_audio_clock_enable_i2s_slave_hclk(false);
    }
    return false;
}

bool hal_audio_device_set_i2s_master(hal_audio_device_parameter_i2s_master_t *handle, hal_audio_control_t device, hal_audio_control_status_t control)
{
    afe_i2s_id_t i2s_id = hal_i2s_convert_id(device, handle->i2s_interface);
    if (handle->rate > 48000) {
        handle->is_low_jitter = true;
    }
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio DIG I2S MASTER:%d, rate:%d, word_length:%d, i2s_format:%d, is_low_jitter:%d, with_mclk:%d, enable:%d", 7, i2s_id, handle->rate, handle->word_length, handle->i2s_format, handle->is_low_jitter, handle->with_mclk, control);

    if (control == HAL_AUDIO_CONTROL_ON) {
        #ifdef AIR_DUAL_CHIP_I2S_ENABLE   //for dual chip project
        hal_i2s_master_gpio_init(i2s_id);
        #endif
        hal_audio_i2s_set_clk(i2s_id, control);
        if (handle->is_low_jitter || handle->with_mclk) {
            afe_i2s_apll_t apll_source = hal_audio_i2s_get_apll_by_samplerate(handle->rate);
            hal_audio_i2s_set_apll(apll_source, HAL_AUDIO_CONTROL_ON);  //Enable APLL
            if (handle->is_low_jitter) {
                hal_audio_i2s_set_low_jitter(apll_source, HAL_AUDIO_CONTROL_ON);
            }
            if (handle->with_mclk) {
                hal_audio_i2s_set_mclk(apll_source, i2s_id, handle->mclk_divider, true);
            }
        }

        hal_i2s_master_set_configuration(handle, i2s_id);
        hal_i2s_master_set_loopback(i2s_id, handle->is_internal_loopback);
        hal_i2s_master_enable(i2s_id, true);
    } else {
        hal_i2s_master_enable(i2s_id, false);

        if (handle->is_low_jitter || handle->with_mclk) {
            afe_i2s_apll_t apll_source = hal_audio_i2s_get_apll_by_samplerate(handle->rate);
            if (handle->with_mclk) {
                hal_audio_i2s_set_mclk(apll_source, i2s_id, handle->mclk_divider, false);
            }
            if (handle->is_low_jitter) {
                hal_audio_i2s_set_low_jitter(apll_source, HAL_AUDIO_CONTROL_OFF);
            }
            hal_audio_i2s_set_apll(apll_source, HAL_AUDIO_CONTROL_OFF); //Disable APLL
        }
        hal_audio_i2s_set_clk(i2s_id, control);
    }
    return false;
}

bool hal_audio_device_set_spdif(hal_audio_device_parameter_spdif_t *handle, hal_audio_control_t device, hal_audio_control_status_t control)
{
    UNUSED(device);
    if (control == HAL_AUDIO_CONTROL_ON) {
        hal_spdif_set_configuration(handle);
        hal_audio_device_set_i2s_master(&handle->i2s_setting, HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER, control);
        hal_spdif_enable(true);
    } else {
        hal_spdif_enable(false);
        hal_audio_device_set_i2s_master(&handle->i2s_setting, HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER, control);
    }
    return false;
}

bool hal_audio_device_set_vad(hal_audio_device_parameter_vad_t *handle, hal_audio_control_t device, hal_audio_control_status_t control)
{
#if (HAL_AUDIO_VAD_DRIVER)
    if (control == HAL_AUDIO_CONTROL_ON) {
        hal_audio_adda_set_bias_enable(handle->bias_select, (hal_audio_bias_voltage_t *)&(handle->bias_voltage), handle->with_bias_lowpower, control);
        hal_audio_ana_set_vad_analog_enable(handle, true);
    } else {
        hal_audio_ana_set_vad_analog_enable(handle, false);
        hal_audio_adda_set_bias_enable(handle->bias_select, (hal_audio_bias_voltage_t *)&(handle->bias_voltage), handle->with_bias_lowpower, control);
    }
    UNUSED(device);
#else
    UNUSED(handle);
    UNUSED(device);
    UNUSED(control);
#endif
    return false;
}

bool hal_audio_device_set_vad_start(hal_audio_vad_start_parameter_t *vad_start)
{
#if (HAL_AUDIO_VAD_DRIVER)
    if (vad_start) {
        if (vad_start->enable) {
            hal_audio_ana_set_vad_digital_enable(NULL, true);
            HAL_AUDIO_TIMER_START(vad_control.timer_handle, HAL_AUDIO_VAD_DELAYON_TIMER_MS);
        } else {
            HAL_AUDIO_TIMER_STOP(vad_control.timer_handle);
            hal_audio_ana_set_vad_digital_enable(NULL, false);
        }
    }
#else
    UNUSED(vad_start);
#endif
    return false;
}

bool hal_audio_device_set_vow(hal_audio_device_parameter_vow_t *handle, hal_audio_control_t device, hal_audio_control_status_t control)
{
    UNUSED(device);
    hal_audio_device_parameter_t device_handle;
    uint8_t performance = handle->adc_parameter.performance;
    vow_control.vow_mode = handle->vow_mode;
    vow_control.first_snr_irq = true;
    vow_control.noise_ignore_bit = handle->noise_ignore_bit;
    hal_wow_power_enable(control);
    hal_vow_reset();
    hal_wow_set_config(handle->vow_with_hpf,handle->snr_threshold,handle->alpha_rise,handle->mic_selection);
    hal_wow_set_dma_irq_threshold(handle->dma_irq_threshold);
    memset(&device_handle,0x0,sizeof(hal_audio_device_parameter_t));
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio VOW mic_selection:0x%x, vow_with_hpf:0x%x, snr_threshold:0x%x, dma_irq_threshold:0x%x on/off:%d",5,handle->mic_selection,handle->vow_with_hpf,handle->snr_threshold,handle->dma_irq_threshold,control);
    if((handle->mic_selection)&HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL){
        hal_wow_set_dmic(handle->vow_mode,handle->dmic_selection,handle->mic_selection,control);
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio VOW dmic_selection:0x%x, vow_mode:0x%x",2,handle->dmic_selection,handle->vow_mode);
    }else if((handle->mic_selection)&HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL){

        afe_analog_select_t analog_select = AFE_ANALOG_ADC0;
        if (handle->mic_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2) {
            analog_select = AFE_ANALOG_ADC1;
        } else if (handle->mic_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3) {
            analog_select = AFE_ANALOG_ADC2;
        }
        #if (HAL_AUDIO_KEEP_ADC_HIGHER_PERFORMANCE_MODE)
        if(afe_analog_control[analog_select].counter)
        {
            performance= hal_get_adc_performance_mode(analog_select);
            HAL_AUDIO_LOG_INFO("VOW use current ADC%d performance mode%d",2,analog_select,handle->adc_parameter.performance);
        }
        #endif
        hal_wow_set_amic(handle->vow_mode,performance,analog_select,handle->mic_selection,control);
        #if 1
        device_handle.analog_mic.audio_device = handle->mic_selection;
        device_handle.analog_mic.adc_parameter.adc_mode = handle->adc_parameter.adc_mode;
        device_handle.analog_mic.adc_parameter.performance = performance;
        device_handle.analog_mic.mic_interface = handle->mic_interface;
        hal_audio_device_analog_set_input((hal_audio_device_parameter_t *)&device_handle, device_handle.analog_mic.audio_device, control);
        #endif
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio VOW analog_select:0x%x, counter:0x%x, vow_mode:0x%x, performance:0x%x",4,analog_select,afe_analog_control[analog_select].counter,handle->vow_mode,handle->adc_parameter.performance);
    }
    hal_nvic_enable_irq(VOW_SNR_IRQn);
    return false;
}

#ifdef AIR_SIDETONE_ENABLE
bool hal_audio_device_set_sidetone(hal_audio_device_parameter_sidetone_t *handle, hal_audio_control_t device, hal_audio_control_status_t control)
{
    bool is_off_done = false;
    UNUSED(device);
    if (control == HAL_AUDIO_CONTROL_ON) {
        if (handle->sidetone_stop_done_entry) {
            HAL_AUDIO_LOG_INFO("DSP - Hal Audio sidetone stop done entry 0x%x", 1, (uint32_t)handle->sidetone_stop_done_entry);
            sidetone_control.sidetone_stop_done_entry = handle->sidetone_stop_done_entry;
        }

        sidetone_control.input_interconn_select = hal_audio_path_get_input_interconnection_port(handle->input_interconn_select);
#ifndef AIR_FIXED_SIDETONE_SOURCE_TO_I2S_RCH_ENABLE
        hal_audio_path_set_interconnection(AUDIO_INTERCONNECTION_CONNECT, HAL_AUDIO_PATH_CHANNEL_DIRECT, sidetone_control.input_interconn_select, AUDIO_INTERCONNECTION_OUTPUT_O10);
#else
        hal_audio_path_set_interconnection(AUDIO_INTERCONNECTION_CONNECT, HAL_AUDIO_PATH_CHANNEL_DIRECT, sidetone_control.input_interconn_select+1, AUDIO_INTERCONNECTION_OUTPUT_O10);
#endif

        if (hal_audio_device_get_rate(hal_device_convert_agent(handle->input_device, handle->input_interface)) !=  handle->rate) {
            sidetone_control.tick_align_enable = true;
            AFE_SET_REG(AUDIO_TOP_CON1, 0<<AUDIO_TOP_CON1_PDN_I2S3_POS, AUDIO_TOP_CON1_PDN_I2S3_MASK);
            AFE_SET_REG(AFE_I2S3_CON, ((afe_samplerate_convert_samplerate_to_register_value(handle->rate))<<AFE_I2S0_CON_RATE_POS)|0x1, AFE_I2S0_CON_RATE_MASK|AFE_I2S0_CON_ENABLE_MASK);
            hal_sidetone_set_input(HAL_AUDIO_AGENT_DEVICE_STF_TICK_ALIGN, true);
        } else {
            hal_sidetone_set_input(hal_device_convert_agent(handle->input_device, handle->input_interface), true);
        }
        hal_sidetone_set_output(hal_device_convert_agent(handle->output_device, handle->output_interface), true);
        hal_sidetone_set_filter(handle->rate,handle->p_sidetone_FIR_coef);
        if (handle->is_sidetone_gain_register_value) {
            sidetone_control.target_negative_gain = handle->sidetone_gain%(AFE_SIDETONE_0DB_REGISTER_VALUE+1);
            sidetone_control.target_positive_gain = handle->sidetone_gain/(AFE_SIDETONE_0DB_REGISTER_VALUE+1);
            HAL_AUDIO_LOG_INFO("sidetone_gain register_value 0x%x",1,handle->sidetone_gain);
        } else {
            sidetone_control.target_negative_gain = hal_sidetone_convert_negative_gain_value(handle->sidetone_gain);
            sidetone_control.target_positive_gain = hal_sidetone_convert_positive_gain_value(handle->sidetone_gain);
            HAL_AUDIO_LOG_INFO("sidetone_gain index %d",1,handle->sidetone_gain);
        }
        sidetone_control.with_ramp_control = handle->with_gain_ramp;
        if (!sidetone_control.with_ramp_control) {
            sidetone_control.current_positive_gain = sidetone_control.target_positive_gain;
            sidetone_control.current_negative_gain = sidetone_control.target_negative_gain;
        }
        hal_sidetone_set_gain_by_register_value(sidetone_control.current_positive_gain, sidetone_control.current_negative_gain);

        hal_sidetone_set_enable(true);
    } else {
        if(handle) {
            sidetone_control.with_ramp_control = handle->with_gain_ramp;
        }
        if (!sidetone_control.ramp_down_done && sidetone_control.with_ramp_control) {
            sidetone_control.target_negative_gain = HAL_AUDIO_SIDETONE_MUTE_NEGATIVE_VALUE;
            sidetone_control.target_positive_gain  = 0;
            HAL_AUDIO_LOG_INFO("sidetone ramp down not done",0);
        } else {
            is_off_done = true;
            hal_sidetone_set_enable(false);
            hal_sidetone_set_input(HAL_AUDIO_AGENT_DEVICE_SIDETONE, false);
            if (sidetone_control.tick_align_enable) {
                sidetone_control.tick_align_enable = false;
                AFE_SET_REG(AFE_I2S3_CON, 0x0, AFE_I2S0_CON_RATE_MASK|AFE_I2S0_CON_ENABLE_MASK);
                AFE_SET_REG(AUDIO_TOP_CON1, 1<<AUDIO_TOP_CON1_PDN_I2S3_POS, AUDIO_TOP_CON1_PDN_I2S3_MASK);
            }
            hal_sidetone_set_output(HAL_AUDIO_AGENT_DEVICE_SIDETONE, false);

#ifndef AIR_FIXED_SIDETONE_SOURCE_TO_I2S_RCH_ENABLE
            hal_audio_path_set_interconnection(AUDIO_INTERCONNECTION_DISCONNECT, HAL_AUDIO_PATH_CHANNEL_DIRECT, sidetone_control.input_interconn_select, AUDIO_INTERCONNECTION_OUTPUT_O10);
#else
            hal_audio_path_set_interconnection(AUDIO_INTERCONNECTION_DISCONNECT, HAL_AUDIO_PATH_CHANNEL_DIRECT, sidetone_control.input_interconn_select+1, AUDIO_INTERCONNECTION_OUTPUT_O10);
#endif

            afe_set_sidetone_enable(false, &dsp_afe_sidetone, true);//close mic and dac
            if (sidetone_control.sidetone_stop_done_entry) {
                sidetone_control.sidetone_stop_done_entry();
            }
        }
    }

    //Set gain step and timer
    if (sidetone_control.with_ramp_control) {
        if ((sidetone_control.target_positive_gain > sidetone_control.current_positive_gain) ||
            (sidetone_control.target_negative_gain > sidetone_control.current_negative_gain)) {
            sidetone_control.gain_step = HAL_AUDIO_SIDETONE_RAMP_UP_STEP;
        } else {
            sidetone_control.gain_step = -(HAL_AUDIO_SIDETONE_RAMP_DOWN_STEP);
        }
        sidetone_control.ramp_down_done = false;
        if (!is_off_done) {
            HAL_AUDIO_LOG_INFO("DSP - Hal Audio sidetone ramp Target:(Pos)%d,(Neg)%d, Current:(Pos)%d,(Neg)%d, Step:%d", 5, sidetone_control.target_positive_gain, sidetone_control.target_negative_gain, sidetone_control.current_positive_gain, sidetone_control.current_negative_gain, sidetone_control.gain_step);
            sidetone_control.ramp_done = false;
#ifdef HAL_AUDIO_SIDETONE_TIMER_CALLBACK
            sidetone_control.ramp_start_delay = (control == HAL_AUDIO_CONTROL_ON);
            HAL_AUDIO_TIMER_START(sidetone_control.timer_handle, (sidetone_control.ramp_start_delay) ? HAL_AUDIO_SIDETONE_ON_DELAY_TIMER_MS : HAL_AUDIO_SIDETONE_RAMP_TIMER_MS);
#else
            while (!sidetone_control.ramp_done) {
                hal_audio_sidetone_timer_callback(sidetone_control.timer_handle);
                HAL_AUDIO_DELAY_US(HAL_AUDIO_SIDETONE_RAMP_TIMER_US);
            }
#endif
        }
    }


    return false;
}
#endif

int32_t hal_get_afe_analog_counter(afe_analog_select_t analog_select){
    return afe_analog_control[analog_select].counter;
}

hal_audio_performance_mode_t hal_get_adc_performance_mode(afe_analog_select_t analog_select){
    hal_audio_analog_mdoe_t adc_mode = HAL_AUDIO_ANALOG_MODE_DUMMY;
    if(afe_analog_control[analog_select].counter){
        adc_mode =  afe_adc_performance_mode[analog_select];
    }else{
        HAL_AUDIO_LOG_INFO("ADC:%d is not enable",1,analog_select);
    }
    return adc_mode;
}

void hal_save_adc_performance_mode(afe_analog_select_t analog_select,uint8_t adc_mode){
    if(afe_analog_control[analog_select].counter){
        afe_adc_performance_mode[analog_select] = adc_mode;
    }else{
        HAL_AUDIO_LOG_INFO("ADC:%d is not enable",1,analog_select);
    }
}

bool hal_audio_device_analog_set_input(hal_audio_device_parameter_t *handle, hal_audio_control_t device, hal_audio_control_status_t control)
{
    afe_analog_control_t analog_control = 0;
    afe_analog_select_t analog_select = AFE_ANALOG_ADC0;
    hal_audio_device_parameter_adc_t *adc_parameter;

    if (device&HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL) {
        adc_parameter = &handle->analog_mic.adc_parameter;
        if (handle->analog_mic.mic_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2) {

            analog_select = AFE_ANALOG_ADC1;
        } else if (handle->analog_mic.mic_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3) {
            analog_select = AFE_ANALOG_ADC2;
        }
    } else if (device&HAL_AUDIO_CONTROL_DEVICE_LINE_IN_DUAL){
        adc_parameter = &handle->linein.adc_parameter;
    } else {
        return true;
    }

    if (device&(HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_L|HAL_AUDIO_CONTROL_DEVICE_LINE_IN_L)) {
        analog_control |= AFE_ANALOG_L_CH;
    }
    if (device&(HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_R|HAL_AUDIO_CONTROL_DEVICE_LINE_IN_R)) {
        analog_control |= AFE_ANALOG_R_CH;
    }

    if (control == HAL_AUDIO_CONTROL_ON) {
        if(afe_analog_control[analog_select].counter==0) {
            analog_control |= AFE_ANALOG_COMMON;
        }
        if(analog_control&AFE_ANALOG_L_CH) {
            if (afe_analog_control[analog_select].channel_counter.channel_l) {
                analog_control &= ~AFE_ANALOG_L_CH;
            }
            afe_analog_control[analog_select].channel_counter.channel_l++;
        }
        if(analog_control&AFE_ANALOG_R_CH) {
            if (afe_analog_control[analog_select].channel_counter.channel_r) {
                analog_control &= ~AFE_ANALOG_R_CH;
            }
            afe_analog_control[analog_select].channel_counter.channel_r++;
        }

        if (analog_control) {
            if(analog_select == AFE_ANALOG_ADC0) {
                hal_audio_ana_set_adc0_enable(adc_parameter, analog_control, true);
            }
            if(analog_select == AFE_ANALOG_ADC1) {
                hal_audio_ana_set_adc23_enable(adc_parameter, analog_control, true);
            }
            if(analog_select == AFE_ANALOG_ADC2) {
                hal_audio_ana_set_adc45_enable(adc_parameter, analog_control, true);
            }
        }
    } else {
        if(analog_control&AFE_ANALOG_L_CH) {
            afe_analog_control[analog_select].channel_counter.channel_l--;
            if (afe_analog_control[analog_select].channel_counter.channel_l) {
                analog_control &= ~AFE_ANALOG_L_CH;
            }
        }
        if(analog_control&AFE_ANALOG_R_CH) {
            afe_analog_control[analog_select].channel_counter.channel_r--;
            if (afe_analog_control[analog_select].channel_counter.channel_r) {
                analog_control &= ~AFE_ANALOG_R_CH;
            }
        }
        if(afe_analog_control[analog_select].counter==0) {
            analog_control |= AFE_ANALOG_COMMON;
        }

        if (analog_control) {
            if (afe_analog_control[analog_select].channel_counter.channel_l!=0) {
                analog_control &= ~AFE_ANALOG_L_CH;
            }
            if (afe_analog_control[analog_select].channel_counter.channel_r!=0) {
                analog_control &= ~AFE_ANALOG_R_CH;
            }
            if(analog_select == AFE_ANALOG_ADC0) {
                hal_audio_ana_set_adc0_enable(adc_parameter, analog_control, false);
            }
            if(analog_select == AFE_ANALOG_ADC1) {
                hal_audio_ana_set_adc23_enable(adc_parameter, analog_control, false);
            }
            if(analog_select == AFE_ANALOG_ADC2) {
                hal_audio_ana_set_adc45_enable(adc_parameter, analog_control, false);
            }
        } else {
            if(afe_analog_control[analog_select].channel_counter.channel_l<0){
                afe_analog_control[analog_select].channel_counter.channel_l = 0;
            }
            if(afe_analog_control[analog_select].channel_counter.channel_r<0){
                afe_analog_control[analog_select].channel_counter.channel_r = 0;
            }
        }
    }

    HAL_AUDIO_LOG_INFO("analog_select=%d,L=%d,R=%d",3,analog_select,afe_analog_control[analog_select].channel_counter.channel_l,afe_analog_control[analog_select].channel_counter.channel_r);

    return false;
}

hal_audio_control_t hal_audio_device_analog_get_output(afe_analog_select_t analog_select)
{
    hal_audio_control_t device = HAL_AUDIO_CONTROL_NONE;
    if (afe_analog_control[analog_select].channel_counter.channel_l > 0) {
        device |= HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_L;
    }
    if (afe_analog_control[analog_select].channel_counter.channel_r > 0) {
        device |= HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_R;
    }
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio current output dac type %d", 1, device);
    return device;
}

bool hal_audio_device_analog_set_output(hal_audio_device_parameter_dac_t *dac_param, hal_audio_control_t device, hal_audio_control_status_t control)
{
    afe_analog_control_t analog_control = 0;
    afe_analog_select_t analog_select = AFE_ANALOG_DAC;


    if (device&(HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_L)) {
        analog_control |= AFE_ANALOG_L_CH;
    }
    if (device&(HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_R)) {
        analog_control |= AFE_ANALOG_R_CH;
    }

    if (control == HAL_AUDIO_CONTROL_ON) {
        if(afe_analog_control[analog_select].counter==0) {
            analog_control |= AFE_ANALOG_COMMON;
        }
        if(analog_control&AFE_ANALOG_L_CH) {
            if (afe_analog_control[analog_select].channel_counter.channel_l) {
                analog_control &= ~AFE_ANALOG_L_CH;
            }
            afe_analog_control[analog_select].channel_counter.channel_l++;
        }
        if(analog_control&AFE_ANALOG_R_CH) {
            if (afe_analog_control[analog_select].channel_counter.channel_r) {
                analog_control &= ~AFE_ANALOG_R_CH;
            }
            afe_analog_control[analog_select].channel_counter.channel_r++;
        }

        if (analog_control) {
            hal_audio_ana_set_dac_enable(dac_param, analog_control, true);
        }
    } else {
        if(analog_control&AFE_ANALOG_L_CH) {
            afe_analog_control[analog_select].channel_counter.channel_l--;
            if (afe_analog_control[analog_select].channel_counter.channel_l) {
                analog_control &= ~AFE_ANALOG_L_CH;
            }
        }
        if(analog_control&AFE_ANALOG_R_CH) {
            afe_analog_control[analog_select].channel_counter.channel_r--;
            if (afe_analog_control[analog_select].channel_counter.channel_r) {
                analog_control &= ~AFE_ANALOG_R_CH;
            }
        }
        if(afe_analog_control[analog_select].counter==0) {
            analog_control |= AFE_ANALOG_COMMON;
        }

        if (analog_control) {
            if (afe_analog_control[analog_select].channel_counter.channel_l!=0) {
                analog_control &= ~AFE_ANALOG_L_CH;
            }
            if (afe_analog_control[analog_select].channel_counter.channel_r!=0) {
                analog_control &= ~AFE_ANALOG_R_CH;
            }

            hal_audio_ana_set_dac_enable(dac_param, analog_control, false);
        } else {
            if(afe_analog_control[analog_select].channel_counter.channel_l<0){
                afe_analog_control[analog_select].channel_counter.channel_l = 0;
            }
            if(afe_analog_control[analog_select].channel_counter.channel_r<0){
                afe_analog_control[analog_select].channel_counter.channel_r = 0;
            }
        }
    }

    HAL_AUDIO_LOG_INFO("analog_out_select=%d,L=%d,R=%d",3,analog_select,afe_analog_control[analog_select].channel_counter.channel_l,afe_analog_control[analog_select].channel_counter.channel_r);
    return false;



   return false;
}



uint32_t hal_audio_device_get_rate(hal_audio_agent_t agent)
{
    uint32_t sample_rate = 0;

    if ((agent >= HAL_AUDIO_AGENT_DEVICE_I2S_MASTER_MIN) && (agent <= HAL_AUDIO_AGENT_DEVICE_I2S_MASTER_MAX)) {
        sample_rate = afe_samplerate_get_i2s_master_samplerate(hal_i2s_convert_id_by_agent(agent));
    } else if ((agent >= HAL_AUDIO_AGENT_DEVICE_I2S_SLAVE_MIN) && (agent <= HAL_AUDIO_AGENT_DEVICE_I2S_SLAVE_MAX)) {

    } else if ((agent > HAL_AUDIO_AGENT_DEVICE_ADDA_DL1) && (agent <= HAL_AUDIO_AGENT_DEVICE_ADDA_MAX)) {
        sample_rate = afe_samplerate_get_ul_samplerate(agent);
    } else if (agent == HAL_AUDIO_AGENT_DEVICE_ADDA_DL1) {
#ifdef MTK_ANC_ENABLE
        if (hal_audio_anc_get_change_dl_rate(&sample_rate) == true) {
            //wait ANC ramp to change DL rate
        } else {
            sample_rate = afe_samplerate_get_dl_samplerate();
        }
#else
        sample_rate = afe_samplerate_get_dl_samplerate();
#endif
    }

    return sample_rate;

}

bool hal_audio_device_set_amp_output_gpio_id(uint32_t gpio)
{
    amp_control.output_gpio = gpio;
    if (amp_control.output_gpio != HAL_AUDIO_AMP_OUTPUT_GPIO_DISABLE){
        HAL_AUDIO_GPIO_INIT(amp_control.output_gpio, HAL_AUDIO_GPIO_PINMUX_GPIO_MODE);
        hal_audio_device_set_output_gpio(HAL_AUDIO_CONTROL_OFF, false);
    }
    return false;
}

bool hal_audio_device_set_mic_bias(hal_audio_mic_bias_parameter_t *mic_bias)
{
    hal_audio_adda_set_bias_enable(mic_bias->bias_select, (hal_audio_bias_voltage_t *)&(mic_bias->bias_voltage), mic_bias->with_bias_lowpower,mic_bias->bias1_2_with_LDO0, mic_bias->enable);
    return false;
}

bool hal_audio_device_set_gpio_on_delay_timer(uint32_t timer_ms)
{
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio set_gpio_on_delay_timer:%d", 1, timer_ms);
    amp_control.delay_handle.delay_gpio_on_time_ms = timer_ms;
    return false;
}

bool hal_audio_device_set_amp_delay_timer(uint32_t timer_ms)
{
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio set_amp_delay_timer:%d", 1, timer_ms);
    amp_control.delay_handle.delay_output_off_time_ms = timer_ms;
    return false;
}

bool hal_audio_device_force_off_delay_timer(void)
{
    if (amp_control.delay_handle.agent != HAL_AUDIO_AGENT_ERROR) {
        hal_audio_amp_delay_off_timer_callback(NULL);
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio force off : 0x%x", 1, amp_control.delay_handle.device_parameter.common.audio_device);
    }
    return false;
}

bool hal_audio_device_set_notice_off_handler(hal_audio_handler_entry handler)
{
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio set_notice_off_handler:0x%x", 1, (unsigned int)handler);
    amp_control.notice_off_handler = handler;
    return false;
}

/*******************************************************************************************
*                                       Memory agent                                       *
********************************************************************************************/
bool hal_audio_memory_set_agent(hal_audio_memory_parameter_t *handle, hal_audio_control_t memory_interface, hal_audio_control_status_t control)
{
    hal_audio_agent_t agent = hal_memory_convert_agent(handle->memory_select);
    UNUSED(memory_interface);
    if (control == HAL_AUDIO_CONTROL_ON) {
        hal_audio_agent_user_count[agent]++;
        if (hal_audio_agent_user_count[agent] == 1) {
            hal_audio_afe_set_enable(true);
            hal_audio_memory_setting(handle, memory_interface, control);
        } else {
            //Get memory agent setting while agent is occupied.
            handle->buffer_addr = hal_memory_get_address(agent);
            handle->buffer_length = hal_memory_get_length(agent);
            HAL_AUDIO_LOG_INFO("DSP - Hal Audio Memory is Occupied agent:%d, Add:0x%x, Length :%d", 3, agent, handle->buffer_addr, handle->buffer_length);
        }

    } else {
        if (hal_audio_agent_user_count[agent] == 1) {
            hal_audio_memory_setting(handle, memory_interface, control);
            hal_audio_afe_set_enable(false);
        } else if (hal_audio_agent_user_count[agent] < 0){
            HAL_AUDIO_LOG_ERROR("DSP - Error Hal Audio Agent Wrong user counter %d !", 1, agent);
            hal_audio_agent_user_count[agent] = 0;
        }
        hal_audio_agent_user_count[agent]--;
    }
    return false;
}

bool hal_audio_memory_setting(hal_audio_memory_parameter_t *handle, hal_audio_control_t memory_interface, hal_audio_control_status_t control)
{
    hal_audio_agent_t agent = hal_memory_convert_agent(handle->memory_select);
    uint32_t allocate_length;
    UNUSED(memory_interface);
    if (control == HAL_AUDIO_CONTROL_ON) {

        //Allocate SRAM
        if (!(handle->buffer_addr)) {
            if (handle->memory_select&(HAL_AUDIO_MEMORY_DL_SRC1|HAL_AUDIO_MEMORY_DL_SRC2)) {
                allocate_length = handle->src_buffer_length;
            } else {
                allocate_length = handle->buffer_length;
                if (handle->pure_agent_with_src) {
                    allocate_length += handle->src_buffer_length;
                }
            }
            handle->buffer_addr = hal_memory_allocate_sram(agent, allocate_length);
            if (handle->pure_agent_with_src) {
                handle->src_buffer_addr = ((uint32_t)handle->buffer_addr + handle->buffer_length);
            } else if (handle->memory_select&(HAL_AUDIO_MEMORY_DL_SRC1|HAL_AUDIO_MEMORY_DL_SRC2)) {
                handle->src_buffer_addr = handle->buffer_addr;
            }
        }

        hal_memory_set_address(handle, agent);


        hal_tick_align_set_irq(agent, true);
        hal_memory_set_irq_period(agent, handle->audio_path_rate, (uint32_t)handle->irq_counter);
        hal_memory_set_irq_enable(agent, control);


        hal_memory_set_samplerate(agent, handle->audio_path_rate);
        hal_memory_set_channel(agent, (handle->with_mono_channel) ? 1 : 2);
        hal_memory_set_format(agent, handle->pcm_format);
        hal_memory_set_align(agent, AFE_MEMORY_ALIGN_24BIT_DATA_8BIT_0);
        hal_memory_set_buffer_mode(agent, AFE_MEMORY_BUFFER_MODE_NORMAL);
        #if 0//move to memory enablnig behind to prevent output buffer of asrc to be reset
        if ((handle->pure_agent_with_src) ||
            ((agent>=HAL_AUDIO_AGENT_MEMORY_SRC_MIN) && (agent<=HAL_AUDIO_AGENT_MEMORY_SRC_MAX))) {
            //Enable SRC
            afe_src_configuration_t src_configuration;
            hal_audio_src_set_parameters(handle, &src_configuration);
            hal_audio_src_configuration(&src_configuration, control);
            hal_audio_src_set_start(&src_configuration, handle->sync_status, control);
        }
        #endif

        if (agent == HAL_AUDIO_AGENT_MEMORY_AWB2) {
            #if (HAL_AUDIO_PATH_ECHO_CONNECTION_MODE == 0)
            //Connect from interconn
            {
                //Enable downSample
                afe_updown_configuration_t updown_configuration;
                //updown_configuration.input_rate = hal_updown_get_input_rate(AFE_UPDOWN_SAMPLER_DOWN_CH23);
                updown_configuration.output_rate = handle->audio_path_rate;
                updown_configuration.is_echo_configure_input = false;
                hal_audio_updown_set_agent(&updown_configuration, AFE_UPDOWN_SAMPLER_DOWN_CH23, control);
            }
            #elif (HAL_AUDIO_PATH_ECHO_CONNECTION_MODE == 1)
            //Connect from UL loopback
            {
                hal_audio_device_parameter_loopback_t loopback;
                loopback.rate = handle->audio_path_rate;
                loopback.audio_device = HAL_AUDIO_CONTROL_DEVICE_LOOPBACK;
                loopback.ul_interface = HAL_AUDIO_CONTROL_DEVICE_INTERFACE_4;
                loopback.iir_filter = HAL_AUDIO_UL_IIR_DISABLE;
                loopback.loopback_setting = AFE_AUDIO_UL_LOOPBACK_FROM_7BIT_SDM;
                hal_audio_device_set_agent((hal_audio_device_parameter_t *)&loopback, loopback.audio_device, control);

                hal_audio_path_set_interconnection((control==HAL_AUDIO_CONTROL_ON) ? AUDIO_INTERCONNECTION_CONNECT : AUDIO_INTERCONNECTION_DISCONNECT, HAL_AUDIO_PATH_CHANNEL_CH01CH02_to_CH01CH02, AUDIO_INTERCONNECTION_INPUT_I36, AUDIO_INTERCONNECTION_OUTPUT_O22);
            }
            #endif
        }

        if (handle->sync_status&HAL_AUDIO_MEMORY_SYNC_MULTIPLE_CHANNEl) {

        }

        if (handle->sync_status&HAL_AUDIO_MEMORY_SYNC_PLAY_EN) {
            /* Workaround:Toggle memory enable to reset current index for waiting play_en */
            hal_memory_set_enable(agent, HAL_AUDIO_CONTROL_ON);
            hal_memory_set_enable(agent, HAL_AUDIO_CONTROL_OFF);

            hal_memory_set_palyen(agent, control);
        } else if (handle->sync_status&HAL_AUDIO_MEMORY_SYNC_SW_TRIGGER) {

        } else if (handle->sync_status&HAL_AUDIO_MEMORY_SYNC_AUDIO_FORWARDER) {

        } else {
            //HAL_AUDIO_MEMORY_SYNC_NONE
            hal_memory_set_enable(agent, control);
        }
        if ((handle->pure_agent_with_src) ||
            ((agent>=HAL_AUDIO_AGENT_MEMORY_SRC_MIN) && (agent<=HAL_AUDIO_AGENT_MEMORY_SRC_MAX))) {
            //Enable SRC
            if (agent == HAL_AUDIO_AGENT_MEMORY_SRC1) {
                pre_src1_empty_gpt_cnt = 0;
            }
            afe_src_configuration_t src_configuration;
            hal_audio_src_set_parameters(handle, &src_configuration);
            hal_audio_src_configuration(&src_configuration, control);
            hal_audio_src_set_start(&src_configuration, handle->sync_status, control);
        }
    } else {
        if (handle->sync_status&HAL_AUDIO_MEMORY_SYNC_PLAY_EN) {
            AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0, 0x0003, 0x0003);
        }
        else {
            hal_memory_set_enable(agent, HAL_AUDIO_CONTROL_ON);
        }
        hal_memory_set_irq_enable(agent, control);
        if ((handle->pure_agent_with_src) ||
        ((agent>=HAL_AUDIO_AGENT_MEMORY_SRC_MIN) && (agent<=HAL_AUDIO_AGENT_MEMORY_SRC_MAX))) {
            if (agent == HAL_AUDIO_AGENT_MEMORY_SRC1) {
                pre_src1_empty_gpt_cnt = 0;
            }
            afe_src_configuration_t src_configuration;
            hal_audio_src_set_parameters(handle, &src_configuration);
            hal_audio_src_set_start(&src_configuration, handle->sync_status, control);
            hal_audio_src_configuration(&src_configuration, control);
        }
        hal_memory_set_enable(agent, control);

        if (agent == HAL_AUDIO_AGENT_MEMORY_AWB2) {
            //Disable downSample
            afe_updown_configuration_t updown_configuration;
            updown_configuration.is_echo_configure_input = false;
            hal_audio_updown_set_agent(&updown_configuration, AFE_UPDOWN_SAMPLER_DOWN_CH23, control);
        }

        if (handle->sync_status&HAL_AUDIO_MEMORY_SYNC_PLAY_EN) {
            hal_memory_set_palyen(agent, control);
            AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0, 0x0000, 0x0003);
        }

        //Free SRAM
        hal_memory_free_sram(agent);
    }
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio Memory:0x%x, Off/On:%d, sync_status:%d", 3, handle->memory_select, control, handle->sync_status);
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio Memory Rate:%d IRQ_cnt:%d, format:%d, is_mono:%d", 4, handle->audio_path_rate, handle->irq_counter, handle->pcm_format, handle->with_mono_channel);
    return false;
}

bool hal_audio_memory_change_irq_period(hal_audio_memory_irq_period_parameter_t *handle)
{
    hal_audio_agent_t agent = hal_memory_convert_agent(handle->memory_select);
    return hal_memory_set_irq_period(agent, handle->rate, handle->irq_counter);
}

bool hal_audio_memory_set_irq_enable(hal_audio_memory_irq_enable_parameter_t *handle)
{
    hal_audio_agent_t agent = hal_memory_convert_agent(handle->memory_select);
    int16_t *counter_ptr = NULL;

    if (handle->memory_select == HAL_AUDIO_MEMORY_UL_MASK) {
        counter_ptr = &afe_control_special_isr_counter;
    }

    if (counter_ptr) {
        if (handle->enable) {
            (*counter_ptr)++;
        } else {
            (*counter_ptr)--;
        }
    }

    if (handle->enable) {
        if ((!counter_ptr) || (*counter_ptr==1) ) {
            hal_tick_align_set_irq(agent, true);
            hal_memory_set_irq_period(agent, handle->rate, (uint32_t)handle->irq_counter);
            hal_memory_set_irq_enable(agent, handle->enable);
        }
    } else {
        if ((!counter_ptr) || (*counter_ptr==0) ) {
            hal_memory_set_irq_enable(agent, handle->enable);
        }
    }
    return false;
}

bool hal_audio_memory_sw_trigger(hal_audio_memory_selection_t memory_select, bool enable)
{
    hal_audio_memory_selection_t search_memory;

    for (search_memory = HAL_AUDIO_MEMORY_DL_SRC1 ; search_memory <= HAL_AUDIO_MEMORY_DL_SRC2 ; search_memory <<= 1) {
        if (!(search_memory&memory_select)) {
            continue;
        }
        if (search_memory&memory_select&(HAL_AUDIO_MEMORY_DL_SRC1|HAL_AUDIO_MEMORY_DL_SRC2)) {
            hal_src_start_continuous_mode(hal_audio_src_get_id(search_memory), false, enable);
        }
    }
    hal_memory_set_enable_by_memory_selection(memory_select, enable);
    return false;
}

hal_audio_memory_selection_t hal_audio_memory_convert_interconn_select_to_memory_selecct(hal_audio_interconn_selection_t interconn_select)
{
    hal_audio_memory_selection_t memory_select = 0;
    switch (interconn_select) {
        case HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL1_CH1:
        case HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL1_CH2:
            memory_select = HAL_AUDIO_MEMORY_UL_VUL1;
            break;
        case HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL2_CH1:
        case HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL2_CH2:
            memory_select = HAL_AUDIO_MEMORY_UL_VUL2;
            break;
        case HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL3_CH1:
        case HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL3_CH2:
            memory_select = HAL_AUDIO_MEMORY_UL_VUL3;
            break;
        case HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_AWB_CH1:
        case HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_AWB_CH2:
            memory_select = HAL_AUDIO_MEMORY_UL_AWB;
            break;
        case HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_AWB2_CH1:
        case HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_AWB2_CH2:
            memory_select = HAL_AUDIO_MEMORY_UL_AWB2;
            break;

        case HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL1_CH1:
        case HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL1_CH2:
            memory_select = HAL_AUDIO_MEMORY_DL_DL1;
            break;
        case HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL2_CH1:
        case HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL2_CH2:
            memory_select = HAL_AUDIO_MEMORY_DL_DL2;
            break;
        case HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL3_CH1:
        case HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL3_CH2:
            memory_select = HAL_AUDIO_MEMORY_DL_DL3;
            break;
        case HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL12_CH1:
        case HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL12_CH2:
            memory_select = HAL_AUDIO_MEMORY_DL_DL12;
            break;
        case HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_SRC1_CH1:
        case HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_SRC1_CH2:
            memory_select = HAL_AUDIO_MEMORY_DL_SRC1;
            break;
        case HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_SRC2_CH1:
        case HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_SRC2_CH2:
            memory_select = HAL_AUDIO_MEMORY_DL_SRC2;
            break;
        default:
            break;
    }
    return memory_select;
}

bool hal_audio_memory_get_info(hal_audio_memory_information_parameter_t *memory_info)
{
    bool is_enable = false;
    if (memory_info) {
        hal_audio_agent_t agent = hal_memory_convert_agent(memory_info->memory_select);
        if (hal_audio_agent_user_count[agent]) {
            is_enable = true;
            memory_info->buffer_addr            = hal_memory_get_address(agent);
            memory_info->buffer_length          = hal_memory_get_length(agent);
            memory_info->buffer_current_offset  = hal_memory_get_offset(agent);
        }
        memory_info->is_enable = is_enable;
    }
    return is_enable;
}

void hal_memory_initialize_sram(void)
{
    uint32_t i;
    if (hal_sram_manager.is_initialized == false) {
        hal_sram_manager.is_initialized = true;

        hal_sram_manager.semaphore_ptr = &(hal_sram_manager.semaphore);
        HAL_AUDIO_SEMAPHORE_INIT(hal_sram_manager.semaphore_ptr, 1);
        HAL_AUDIO_SEMAPHORE_TAKE(hal_sram_manager.semaphore_ptr);

        for (i=0 ; i<HAL_AUDIO_MEMORY_SRAM_BLOCK_NUMBER ; i++) {
            hal_sram_manager.block[i].user = HAL_AUDIO_AGENT_ERROR;
            hal_sram_manager.block[i].sram_addr = HAL_AUDIO_MEMORY_SRAM_BASE+(i*HAL_AUDIO_MEMORY_SRAM_BLOCK_SIZE);

        }
        hal_sram_manager.remain_block = HAL_AUDIO_MEMORY_SRAM_BLOCK_NUMBER;
        HAL_AUDIO_SEMAPHORE_GIVE(hal_sram_manager.semaphore_ptr);
    }
}

uint32_t hal_memory_allocate_sram(hal_audio_agent_t user_type, uint32_t size)
{
    uint32_t get_ptr = 0, allow_size = 0;
    int32_t search_block, bottom_index, top_index, search_direction;

    if ((user_type == HAL_AUDIO_AGENT_MEMORY_DL2) ||
        (user_type == HAL_AUDIO_AGENT_MEMORY_SRC2) ||
        (user_type == HAL_AUDIO_AGENT_MEMORY_DL3)  ||
        (user_type == HAL_AUDIO_AGENT_MEMORY_DL12)  ||
        (user_type == HAL_AUDIO_AGENT_MEMORY_VUL3)) {
        //backward usage
        search_direction = -1;
        search_block = HAL_AUDIO_MEMORY_SRAM_BLOCK_NUMBER-1;

    } else {
        //forward usage
        search_direction = 1;
        search_block = 0;
    }

    HAL_AUDIO_SEMAPHORE_TAKE(hal_sram_manager.semaphore_ptr);
    top_index = bottom_index = 0;
    for ( ; (search_block>=0)&&(search_block<HAL_AUDIO_MEMORY_SRAM_BLOCK_NUMBER) ; search_block+=search_direction) {
        if (hal_sram_manager.block[search_block].user == HAL_AUDIO_AGENT_ERROR) {
            if (allow_size==0) {
                if (search_direction>0) {
                    top_index = search_block;
                } else {
                    bottom_index = search_block;
                }
            }
            allow_size += HAL_AUDIO_MEMORY_SRAM_BLOCK_SIZE;
        } else {
            allow_size = 0;
        }

        if (allow_size>=size) {
            //Find available SRAM
            if (search_direction>0) {
                bottom_index = search_block;
            } else {
                top_index = search_block;
            }
            break;
        }
    }


    if (allow_size>=size) {
        for (search_block=top_index ; search_block <= bottom_index ; search_block++) {
            hal_sram_manager.block[search_block].user = user_type;
            hal_sram_manager.remain_block--;
        }
        HAL_AUDIO_SEMAPHORE_GIVE(hal_sram_manager.semaphore_ptr);
        hal_audio_afe_set_enable(true);//add for issue BTA-6421,Before using aduio sram, you need to enable afe.
        get_ptr = hal_sram_manager.block[top_index].sram_addr;
        memset((U8*)get_ptr, 0, size);
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio Memory SRAM alloc Type:%d, size:%d, block index:%d, Reamin_block:%d", 4, user_type, size, top_index, hal_sram_manager.remain_block);
    } else {
        HAL_AUDIO_SEMAPHORE_GIVE(hal_sram_manager.semaphore_ptr);
        HAL_AUDIO_LOG_ERROR("DSP - Error Hal Audio Memory SRAM alloc fail. Type:%d, size:%d, Reamin_block:%d", 3, user_type, size, hal_sram_manager.remain_block);
        OS_ASSERT(FALSE);
    }
    //LOG_PRINT_AUDIO("DSP SRAM Allocate Type:%d, 7, size:%d, block index:%d, Reamin_block:%d", user_type, size, top_index, hal_sram_manager.remain_block);
    return get_ptr;
}

uint32_t hal_memory_free_sram(hal_audio_agent_t user_type)
{
    uint32_t block_number = 0, top_index = 0;
    int32_t search_block;

    HAL_AUDIO_SEMAPHORE_TAKE(hal_sram_manager.semaphore_ptr);

    for (search_block=0 ; search_block < HAL_AUDIO_MEMORY_SRAM_BLOCK_NUMBER ; search_block++) {
        if (hal_sram_manager.block[search_block].user == user_type) {
            hal_sram_manager.block[search_block].user = HAL_AUDIO_AGENT_ERROR;
            hal_sram_manager.remain_block++;

            if (block_number==0) {
                top_index = search_block;
            }
            block_number++;
        }
    }
    if(block_number){//found user
        hal_audio_afe_set_enable(false);//add for issue BTA-6421,Before using aduio sram, you need to enable afe.
    }
    HAL_AUDIO_SEMAPHORE_GIVE(hal_sram_manager.semaphore_ptr);
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio Memory SRAM free  Type:%d, free number:%d, block index:%d, Reamin_block:%d", 4, user_type, block_number, top_index, hal_sram_manager.remain_block);
    //LOG_PRINT_AUDIO("DSP SRAM Free Type:%d, 7, block_number:%d, block index:%d, Reamin_block:%d", user_type, block_number, top_index, hal_sram_manager.remain_block);
    return block_number;
}


/*******************************************************************************************
*                                            SRC                                           *
********************************************************************************************/
afe_asrc_id_t hal_audio_src_get_id(hal_audio_memory_selection_t memory_select)
{
    afe_asrc_id_t src_id;
    if (memory_select&(HAL_AUDIO_MEMORY_DL_SRC2|HAL_AUDIO_MEMORY_DL_DL2)) {
        src_id = AFE_SRC_2;
    } else {
        src_id = AFE_SRC_1;
        #ifdef AIR_HWSRC_RX_TRACKING_ENABLE
        if(memory_select & HAL_AUDIO_MEMORY_UL_VUL1){
            src_id = AFE_SRC_2;
        }else if(memory_select & HAL_AUDIO_MEMORY_UL_VUL3){
        #ifdef AIR_DUAL_CHIP_MASTER_HWSRC_RX_TRACKING_ENABLE
            src_id = AFE_SRC_1;
        #else
            src_id = AFE_SRC_2;
        #endif
        }
        #endif
    }
    return src_id;
}

bool hal_audio_src_initialize_compensation(afe_asrc_id_t asrc_id)
{
    afe_src_compensation[asrc_id].base_value = hal_src_get_src_input_rate(asrc_id);
    afe_src_compensation[asrc_id].step_value = (afe_src_compensation[asrc_id].base_value)>>13;
    afe_src_compensation[asrc_id].compensation_value = 0;
    afe_src_compensation[asrc_id].step = 0;
    return false;
}

int32_t hal_audio_control_get_src_xppm(hal_audio_src_compensation_parameter_t *src_compensation)
{
    afe_asrc_id_t asrc_id = hal_audio_src_get_id(src_compensation->memory_select);
    int32_t xppm = 0;

    if (afe_src_compensation[asrc_id].base_value)
    {
        xppm = 1000000 * (S64)(afe_src_compensation[asrc_id].compensation_value) / afe_src_compensation[asrc_id].base_value;
    }

    return xppm;
}

uint32_t hal_audio_control_get_src_input_sample_count(hal_audio_memory_selection_t *memory_sselect)
{
    afe_asrc_id_t asrc_id = hal_audio_src_get_id(*memory_sselect);
    return hal_src_get_src_input_sample_count(asrc_id);
}

bool hal_audio_control_set_src_compensation(hal_audio_src_compensation_parameter_t *src_compensation)
{
    afe_asrc_id_t asrc_id = hal_audio_src_get_id(src_compensation->memory_select);
    if (src_compensation->control == HAL_AUDIO_SRC_COMPENSATION_SET_VALUE) {
        afe_src_compensation[asrc_id].compensation_value = src_compensation->compensation_value;
    } else {
        if (src_compensation->control == HAL_AUDIO_SRC_COMPENSATION_INCREASE) {
            afe_src_compensation[asrc_id].step++;
            if (afe_src_compensation[asrc_id].step > HAL_AUDIO_SRC_COMPENSATION_MAX_STEP>>1) {
                afe_src_compensation[asrc_id].step++;
            }
        } else if (src_compensation->control == HAL_AUDIO_SRC_COMPENSATION_DECREASE) {
            afe_src_compensation[asrc_id].step--;
            if (afe_src_compensation[asrc_id].step < -(HAL_AUDIO_SRC_COMPENSATION_MAX_STEP>>1)) {
                afe_src_compensation[asrc_id].step--;
            }
        } else {
            afe_src_compensation[asrc_id].compensation_value = 0;
        }

        if (afe_src_compensation[asrc_id].step > HAL_AUDIO_SRC_COMPENSATION_MAX_STEP) {
            afe_src_compensation[asrc_id].step = HAL_AUDIO_SRC_COMPENSATION_MAX_STEP;
        } else if (afe_src_compensation[asrc_id].step < -HAL_AUDIO_SRC_COMPENSATION_MAX_STEP) {
            afe_src_compensation[asrc_id].step = -HAL_AUDIO_SRC_COMPENSATION_MAX_STEP;
        }
        afe_src_compensation[asrc_id].compensation_value = afe_src_compensation[asrc_id].step * afe_src_compensation[asrc_id].step_value;
    }

    hal_src_set_src_input_rate(asrc_id, (uint32_t)(afe_src_compensation[asrc_id].base_value + afe_src_compensation[asrc_id].compensation_value));
    return false;
}


bool hal_audio_src_set_parameters(hal_audio_memory_parameter_t *handle, afe_src_configuration_t *configuration)
{
    afe_src_buffer_t *src_buffer_info;
    uint32_t byte_per_sample;
    byte_per_sample = (handle->pcm_format > HAL_AUDIO_PCM_FORMAT_U16_BE) ? 4 : 2;

    configuration->hw_update_obuf_rdpnt = false;
    configuration->is_mono = handle->with_mono_channel;
    configuration->id = hal_audio_src_get_id(handle->memory_select);
    configuration->sample_count_threshold = handle->irq_counter;
#ifdef ENABLE_HWSRC_CLKSKEW
    configuration->clkskew_mode = handle->asrc_clkskew_mode;
#endif

    if (handle->memory_select&HAL_AUDIO_MEMORY_UL_MASK) {
        configuration->ul_mode = true;
        if (handle->src_tracking_clock_source) {
            configuration->mode = AFE_SRC_TRACKING_MODE_RX;
            configuration->tracking_clock = handle->src_tracking_clock_source;
        } else {
            configuration->mode = AFE_SRC_NO_TRACKING;
        }

    } else {
        configuration->ul_mode = false;
        if (handle->memory_select&(HAL_AUDIO_MEMORY_DL_SRC1|HAL_AUDIO_MEMORY_DL_SRC2)) {
            configuration->mode = AFE_SRC_CONTINUOUS;
        } else {
            configuration->hw_update_obuf_rdpnt = true;
            if (handle->src_tracking_clock_source) {
                configuration->mode = AFE_SRC_TRACKING_MODE_TX;
                configuration->tracking_clock = handle->src_tracking_clock_source;
            } else {
                configuration->mode = AFE_SRC_NO_TRACKING;
            }
        }
    }

    //Configure stream port buffer
    if (configuration->ul_mode) {
        src_buffer_info = &configuration->output_buffer;
    } else {
        src_buffer_info = &configuration->input_buffer;
    }
    src_buffer_info->addr = handle->src_buffer_addr;
    src_buffer_info->size = handle->src_buffer_length;
    src_buffer_info->rate = handle->src_rate;
    src_buffer_info->offset = handle->initial_buffer_offset;
    src_buffer_info->format = handle->pcm_format;


    //Configure device port buffer
    if (configuration->ul_mode) {
        src_buffer_info = &configuration->input_buffer;
    } else {
        src_buffer_info = &configuration->output_buffer;
    }
    src_buffer_info->addr = handle->buffer_addr;
    src_buffer_info->size = handle->buffer_length;
    src_buffer_info->rate = handle->audio_path_rate;
    src_buffer_info->offset = 32;
    src_buffer_info->format = handle->pcm_format;


    HAL_AUDIO_LOG_INFO("DSP - Hal Audio SRC mode:%d, id:%d, in:%d, out:%d", 4, configuration->mode, configuration->id, configuration->input_buffer.rate, configuration->output_buffer.rate);
    return false;
}
bool hal_audio_src_enable_clock(afe_asrc_id_t asrc_id, hal_audio_control_status_t control)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (control==HAL_AUDIO_CONTROL_ON) {
        hal_audio_clock_enable_src(true);
        if (asrc_id == AFE_SRC_1) {
            hal_audio_clock_enable_src1(true);
        } else {
            hal_audio_clock_enable_src2(true);
        }
    } else {
        if (asrc_id == AFE_SRC_1) {
            hal_audio_clock_enable_src1(false);
        } else {
            hal_audio_clock_enable_src2(false);
        }
        hal_audio_clock_enable_src(false);
    }
    HAL_AUDIO_EXIT_CRITICAL();
    return false;
}

void hal_audio_src_reset_rate(afe_asrc_id_t asrc_id){
    //reset rate = 0
    if(asrc_id == AFE_SRC_1){
        AFE_SET_REG(ASM_FREQUENCY_2, 0 ,0xFFFFFF);
        HAL_AUDIO_LOG_INFO("[HWSRC]reset hwsrc1 tracking rate done,ASM_FREQUENCY_2=0x%x",1,AFE_READ(ASM_FREQUENCY_2));
    }else{
        AFE_SET_REG(ASM2_FREQUENCY_2, 0 ,0xFFFFFF);
        HAL_AUDIO_LOG_INFO("[HWSRC]reset hwsrc2 tracking rate done,ASM2_FREQUENCY_2=0x%x",1,AFE_READ(ASM2_FREQUENCY_2));
    }
}

bool hal_audio_src_configuration(afe_src_configuration_t *configuration, hal_audio_control_status_t control)
{
    if (control==HAL_AUDIO_CONTROL_ON) {
        hal_audio_src_enable_clock(configuration->id, control);
        //hal_src_set_power(configuration->id, true);
        hal_src_set_iir(configuration->id, configuration->input_buffer.rate, configuration->output_buffer.rate);
        hal_src_set_configuration(configuration, true);
        hal_audio_src_initialize_compensation(configuration->id);

        hal_src_set_continuous(configuration, true);
    } else {
        hal_src_set_continuous(configuration, false);
        //hal_src_set_power(configuration->id, false);
        //hal_audio_src_reset_rate(configuration->id);
        hal_audio_src_enable_clock(configuration->id, control);
    }
    return false;
}

bool hal_audio_src_set_start(afe_src_configuration_t *configuration, hal_audio_memory_sync_selection_t sync_select, hal_audio_control_status_t control)
{
    if (control==HAL_AUDIO_CONTROL_ON) {
        hal_src_set_start(configuration->id, true);//move to afe_dl1_interrupt_handler and afe_dl2_interrupt_handler
        if ((configuration->mode == AFE_SRC_CONTINUOUS) &&
            !(sync_select&(HAL_AUDIO_MEMORY_SYNC_SW_TRIGGER|HAL_AUDIO_MEMORY_SYNC_AUDIO_FORWARDER))){
            hal_src_start_continuous_mode(configuration->id, (sync_select&HAL_AUDIO_MEMORY_SYNC_PLAY_EN) ? true : false, true);
        }
    } else {
        hal_src_start_continuous_mode(configuration->id, false, false);
        hal_src_set_start(configuration->id, false);
    }
    return false;
}

bool hal_audio_src_trigger_start(afe_asrc_id_t id)
{
    hal_src_start_continuous_mode(id, false, true);
    return false;
}

/*******************************************************************************************
*                                         HW gain                                          *
********************************************************************************************/
bool hal_audio_hardware_gain_set_agent (afe_hardware_digital_gain_t gain_select, uint32_t samplerate, hal_audio_control_status_t control)
{
    hal_audio_agent_t hw_gain_agent;
    if (gain_select == AFE_HW_DIGITAL_GAIN1) {
        hw_gain_agent = HAL_AUDIO_AGENT_BLOCK_HWGAIN1;
    } else if (gain_select == AFE_HW_DIGITAL_GAIN2) {
        hw_gain_agent = HAL_AUDIO_AGENT_BLOCK_HWGAIN2;
    } else {
        hw_gain_agent = HAL_AUDIO_AGENT_BLOCK_HWGAIN3;
    }
    if (control == HAL_AUDIO_CONTROL_ON) {
        hal_audio_agent_user_count[hw_gain_agent]++;
        if (hal_audio_agent_user_count[hw_gain_agent]== 1) {
            hal_hw_gain_set_enable(gain_select, samplerate, true);
            #if 1
            //HW gain enable deadlock Workaround
            if ((hw_gain_agent==HAL_AUDIO_AGENT_BLOCK_HWGAIN1) && (hal_audio_agent_user_count[HAL_AUDIO_AGENT_BLOCK_HWGAIN2]==0)) {
                hal_hw_gain_set_enable(AFE_HW_DIGITAL_GAIN2, samplerate, true);
            } else if ((hw_gain_agent==HAL_AUDIO_AGENT_BLOCK_HWGAIN2) && (hal_audio_agent_user_count[HAL_AUDIO_AGENT_BLOCK_HWGAIN1]==0)) {
                hal_hw_gain_set_enable(AFE_HW_DIGITAL_GAIN1, samplerate, true);
            }
            #endif
            afe_volume_digital_set_mute(gain_select, AFE_VOLUME_MUTE_BLOCK_DISABLE, false);
            //device on to apply HW gain compensation value
            if(hal_audio_agent_user_count[HAL_AUDIO_AGENT_BLOCK_HWGAIN1]){
               afe_volume_digital_update(AFE_HW_DIGITAL_GAIN1);
            }
            if(hal_audio_agent_user_count[HAL_AUDIO_AGENT_BLOCK_HWGAIN2]){
               afe_volume_digital_update(AFE_HW_DIGITAL_GAIN2);
            }
            if(hal_audio_agent_user_count[HAL_AUDIO_AGENT_BLOCK_HWGAIN3]){
               afe_volume_digital_update(AFE_HW_DIGITAL_GAIN3);
            }
        } else if (hal_audio_agent_user_count[hw_gain_agent] > 2) {
            HAL_AUDIO_LOG_WARNING("DSP - Warning HW digital gain is occupancy %d @@", 1, gain_select);
        }
    }
    else {
        hal_audio_agent_user_count[hw_gain_agent]--;
        if (hal_audio_agent_user_count[hw_gain_agent] == 0) {
            afe_volume_digital_set_mute(gain_select, AFE_VOLUME_MUTE_BLOCK_DISABLE, true);
            if (hw_gain_agent == HAL_AUDIO_AGENT_BLOCK_HWGAIN3) {
                hal_hw_gain_set_enable(AFE_HW_DIGITAL_GAIN3, samplerate, false);
            } else if (!(hal_audio_agent_user_count[HAL_AUDIO_AGENT_BLOCK_HWGAIN1] || hal_audio_agent_user_count[HAL_AUDIO_AGENT_BLOCK_HWGAIN2])) {
                hal_hw_gain_set_enable(AFE_HW_DIGITAL_GAIN1, samplerate, false);
                hal_hw_gain_set_enable(AFE_HW_DIGITAL_GAIN2, samplerate, false);
            }

            //device off, the hardware gain compensation value needs to be removed to restore the original value
            if(hal_audio_agent_user_count[HAL_AUDIO_AGENT_BLOCK_HWGAIN1]){
               afe_volume_digital_update(AFE_HW_DIGITAL_GAIN1);
            }
            if(hal_audio_agent_user_count[HAL_AUDIO_AGENT_BLOCK_HWGAIN2]){
               afe_volume_digital_update(AFE_HW_DIGITAL_GAIN2);
            }
            if(hal_audio_agent_user_count[HAL_AUDIO_AGENT_BLOCK_HWGAIN3]){
               afe_volume_digital_update(AFE_HW_DIGITAL_GAIN3);
            }

        } else if (hal_audio_agent_user_count[hw_gain_agent] < 0) {
            hal_audio_agent_user_count[hw_gain_agent] = 0;
        }
    }
    return false;
}

int32_t hal_audio_hardware_gain_get_agent_count (afe_hardware_digital_gain_t gain_select)
{
    hal_audio_agent_t hw_gain_agent;
    if (gain_select == AFE_HW_DIGITAL_GAIN1) {
        hw_gain_agent = HAL_AUDIO_AGENT_BLOCK_HWGAIN1;
    } else if (gain_select == AFE_HW_DIGITAL_GAIN2) {
        hw_gain_agent = HAL_AUDIO_AGENT_BLOCK_HWGAIN2;
    } else {
        hw_gain_agent = HAL_AUDIO_AGENT_BLOCK_HWGAIN3;
    }
    return hal_audio_agent_user_count[hw_gain_agent];
}


afe_hardware_digital_gain_t hal_audio_hardware_gain_get_selcet (hal_audio_memory_selection_t memory_selection)
{
    afe_hardware_digital_gain_t gain_select;
    if (memory_selection&(HAL_AUDIO_MEMORY_DL_DL1|HAL_AUDIO_MEMORY_DL_SRC1)) {
        gain_select = AFE_HW_DIGITAL_GAIN1;
    } else if (memory_selection&(HAL_AUDIO_MEMORY_DL_DL2|HAL_AUDIO_MEMORY_DL_SRC2)) {
        gain_select = AFE_HW_DIGITAL_GAIN2;
    } else {
        gain_select = AFE_HW_DIGITAL_GAIN3;
    }
    return gain_select;
}

/*******************************************************************************************
*                                       I2S master                                         *
********************************************************************************************/
afe_i2s_apll_t hal_audio_i2s_get_apll_by_samplerate(uint32_t samplerate)
{
    if (samplerate == 176400 || samplerate == 88200 || samplerate == 44100 || samplerate == 22050 || samplerate == 11025) {
        return AFE_I2S_APLL1;
    }
    else {
        return AFE_I2S_APLL2;
    }
}

bool hal_audio_i2s_set_apll(afe_i2s_apll_t apll_source, hal_audio_control_status_t control)
{
    if (control == HAL_AUDIO_CONTROL_ON) {
        hal_i2s_master_enable_apll(apll_source, true);
    } else {
        hal_i2s_master_enable_apll(apll_source, false);
    }
    return false;
}

bool hal_audio_i2s_set_low_jitter(afe_i2s_apll_t apll_source, hal_audio_control_status_t control)
{
#if 1
    if (apll_source == AFE_I2S_APLL1) {
        hal_audio_clock_enable_22m(control);
    } else {
        hal_audio_clock_enable_24m(control);
    }
#else
    UNUSED(apll_source);
    hal_audio_clock_enable_22m(control);
    hal_audio_clock_enable_24m(control);
    #if 0//2822 clock control on CM4, modify for ab1568
    if (control == HAL_AUDIO_CONTROL_ON) {
        clock_mux_sel(CLK_AUD_INTERFACE0_SEL, 2);   //AFE_SET_REG(CKSYS_CLK_CFG_4, 2, 0x3);
        hal_clock_enable(HAL_CLOCK_CG_AUD_INTF0);
    } else {
        hal_clock_disable(HAL_CLOCK_CG_AUD_INTF0);  //AFE_SET_REG(CKSYS_CLK_CFG_4, 1, 0x3);
    }
    #endif
#endif

    return false;
}

bool hal_audio_i2s_set_mclk(afe_i2s_apll_t apll_source, afe_i2s_id_t i2s_id, uint32_t mclk_divider, bool enable)
{
    hal_i2s_master_enable_mclk(apll_source, i2s_id, mclk_divider, enable);
    return false;
}

bool hal_audio_i2s_set_clk(afe_i2s_id_t i2s_id, bool enable)
{
    switch(i2s_id) {
        case AFE_I2S0:
            hal_audio_clock_enable_i2s0(enable);
            break;
        case AFE_I2S1:
            hal_audio_clock_enable_i2s1(enable);
            break;
        case AFE_I2S2:
            hal_audio_clock_enable_i2s2(enable);
            break;
        default:
            return true;
            break;
    }
    return false;
}

/*******************************************************************************************
*                                       I2S slave                                          *
********************************************************************************************/
#ifndef MTK_TDM_ENABLE
extern vdma_channel_t g_i2s_slave_vdma_channel_infra[];

bool hal_audio_slave_set_vdma(hal_audio_slave_vdma_parameter_t *vdma_setting)
{
    vdma_config_t dma_config;
    vdma_channel_t dma_channel;
    vdma_status_t i2s_vdma_status;
    uint32_t port, threshold;

    if (vdma_setting->audio_interface == HAL_AUDIO_INTERFACE_1) {
        port = 0;
    } else if (vdma_setting->audio_interface == HAL_AUDIO_INTERFACE_2) {
        port = 1;
    } else if (vdma_setting->audio_interface == HAL_AUDIO_INTERFACE_3) {
        port = 2;
    } else {
        DSP_MW_LOG_I("[SLAVE VDMA] audio_interface = %d fail", 1, vdma_setting->audio_interface);
        return -1;
    }

    dma_channel = g_i2s_slave_vdma_channel_infra[port * 2 + vdma_setting->is_ul_mode];

    dma_config.base_address = vdma_setting->base_address;
    dma_config.size = vdma_setting->size;
    threshold = vdma_setting->threshold;

    DSP_MW_LOG_I("[SLAVE VDMA] dma_channel=%d, threshold=%d, base_address=0x%x, size=%d", 4,dma_channel,threshold,dma_config.base_address,dma_config.size);

    i2s_vdma_status = vdma_init(dma_channel);
    if (i2s_vdma_status != VDMA_OK) {
        DSP_MW_LOG_I("[SLAVE VDMA] set vdma_init fail %d", 1, i2s_vdma_status);
        return -1;
    }

    i2s_vdma_status = vdma_enable_interrupt(dma_channel);
    if (i2s_vdma_status != VDMA_OK) {
        DSP_MW_LOG_I("[SLAVE VDMA] set vdma_enable_interrupt fail %d", 1, i2s_vdma_status);
        return -1;
    }

    i2s_vdma_status = vdma_configure(dma_channel, &dma_config);
    if (i2s_vdma_status != VDMA_OK) {
        DSP_MW_LOG_I("[SLAVE VDMA] set vdma_configure fail %d", 1, i2s_vdma_status);
        return -1;
    }

    i2s_vdma_status = vdma_set_threshold(dma_channel, threshold);
    if (i2s_vdma_status != VDMA_OK) {
        DSP_MW_LOG_I("[SLAVE VDMA] set vdma_set_threshold fail %d", 1, i2s_vdma_status);
        return -1;
    }

    i2s_vdma_status = vdma_start(dma_channel);
    if (i2s_vdma_status != VDMA_OK) {
        DSP_MW_LOG_I("[SLAVE VDMA] set vdma_start fail %d", 1, i2s_vdma_status);
        return -1;
    }

    return 0;
}
#else
extern vdma_channel_t g_i2s_slave_vdma_channel_infra[];
extern vdma_channel_t g_i2s_slave_vdma_channel_tdm[];
bool hal_audio_slave_set_vdma(hal_audio_slave_vdma_parameter_t *vdma_setting)
{
    vdma_config_t dma_config;
    vdma_channel_t dma_channel, dma_set_ch0, dma_set_ch1, dma_set_ch2, dma_set_ch3;
    vdma_status_t i2s_vdma_status;
    uint32_t mask, port, setting_cnt, dma_setting_count;

    DSP_MW_LOG_I("[SLAVE VDMA] base_address=0x%x, size=%d, threshold=%d, tdm_channel=%d", 4, vdma_setting->base_address, vdma_setting->size, vdma_setting->threshold, vdma_setting->tdm_channel);

    hal_nvic_save_and_set_interrupt_mask(&mask);

    if (vdma_setting->tdm_channel == HAL_AUDIO_I2S_TDM_DISABLE) {
        if (vdma_setting->audio_interface == HAL_AUDIO_INTERFACE_1) {
            port = 0;
        } else if (vdma_setting->audio_interface == HAL_AUDIO_INTERFACE_2) {
            port = 1;
        } else if (vdma_setting->audio_interface == HAL_AUDIO_INTERFACE_3) {
            port = 2;
        } else {
            DSP_MW_LOG_I("[SLAVE VDMA] audio_interface = %d fail", 1, vdma_setting->audio_interface);
            return -1;
        }
        dma_set_ch0 = g_i2s_slave_vdma_channel_infra[port * 2 + vdma_setting->is_ul_mode];
        dma_setting_count = 1;
    } else {
        if (vdma_setting->audio_interface == HAL_AUDIO_INTERFACE_2) {
            port = 0;
        } else if (vdma_setting->audio_interface == HAL_AUDIO_INTERFACE_3) {
            port = 1;
        } else {
            DSP_MW_LOG_I("[SLAVE VDMA] audio_interface = %d fail", 1, vdma_setting->audio_interface);
            return -1;
        }
        dma_set_ch0 = g_i2s_slave_vdma_channel_tdm[port * 8 + vdma_setting->is_ul_mode];
        if (vdma_setting->tdm_channel >= HAL_AUDIO_I2S_TDM_4CH) {
            dma_set_ch1 = g_i2s_slave_vdma_channel_tdm[port * 8 + vdma_setting->is_ul_mode + 2];
            dma_setting_count = 2;
        }
        if (vdma_setting->tdm_channel >= HAL_AUDIO_I2S_TDM_6CH) {
            dma_set_ch2 = g_i2s_slave_vdma_channel_tdm[port * 8 + vdma_setting->is_ul_mode + 4];
            dma_setting_count = 3;
        }
        if (vdma_setting->tdm_channel >= HAL_AUDIO_I2S_TDM_8CH) {
            dma_set_ch3 = g_i2s_slave_vdma_channel_tdm[port * 8 + vdma_setting->is_ul_mode + 6];
            dma_setting_count = 4;
        }
    }

    for (setting_cnt=0; setting_cnt < dma_setting_count; setting_cnt++) {
        if (setting_cnt==0) {
            dma_channel = dma_set_ch0;
        } else if (setting_cnt==1) {
            dma_channel = dma_set_ch1;
        } else if (setting_cnt==2) {
            dma_channel = dma_set_ch2;
        } else {
            dma_channel = dma_set_ch3;
        }
        dma_config.base_address = vdma_setting->base_address + vdma_setting->size*4*setting_cnt;
        dma_config.size = vdma_setting->size;
        DSP_MW_LOG_I("[SLAVE VDMA] dma_channel=%d, dma_config.base_address=0x%x, setting_cnt=%d, dma_setting_count=%d", 4,dma_channel,dma_config.base_address,setting_cnt,dma_setting_count);
        i2s_vdma_status = vdma_init(dma_channel);
        if (i2s_vdma_status != VDMA_OK) {
            DSP_MW_LOG_I("[SLAVE VDMA] set vdma_init fail %d", 1, i2s_vdma_status);
            return -1;
        }
        i2s_vdma_status = vdma_enable_interrupt(dma_channel);
        if (i2s_vdma_status != VDMA_OK) {
            DSP_MW_LOG_I("[SLAVE VDMA] set vdma_enable_interrupt fail %d", 1, i2s_vdma_status);
            return -1;
        }
        i2s_vdma_status = vdma_configure(dma_channel, &dma_config);
        if (i2s_vdma_status != VDMA_OK) {
            DSP_MW_LOG_I("[SLAVE VDMA] set vdma_configure fail %d", 1, i2s_vdma_status);
            return -1;
        }
        i2s_vdma_status = vdma_set_threshold(dma_channel, vdma_setting->threshold);
        if (i2s_vdma_status != VDMA_OK) {
            DSP_MW_LOG_I("[SLAVE VDMA] set vdma_set_threshold fail %d", 1, i2s_vdma_status);
            return -1;
        }
    }

    for (setting_cnt=0; setting_cnt < dma_setting_count; setting_cnt++) {
        if (setting_cnt==0) {
            dma_channel = dma_set_ch0;
        } else if (setting_cnt==1) {
            dma_channel = dma_set_ch1;
        } else if (setting_cnt==2) {
            dma_channel = dma_set_ch2;
        } else {
            dma_channel = dma_set_ch3;
        }
        i2s_vdma_status = vdma_start(dma_channel);
        if (i2s_vdma_status != VDMA_OK) {
            DSP_MW_LOG_I("[SLAVE VDMA] set vdma_start fail %d", 1, i2s_vdma_status);
            return -1;
        }
    }

    hal_nvic_restore_interrupt_mask(mask);
    return 0;
}
#endif

/*******************************************************************************************
*                                    Sine Generator                                        *
********************************************************************************************/
uint32_t hal_audio_control_set_sine_generator (hal_audio_sine_generator_parameter_t *generator_handle)
{
    hal_audio_agent_t agent;
    if (generator_handle->audio_control == HAL_AUDIO_CONTROL_MEMORY_INTERFACE) {
        agent = hal_memory_convert_agent(generator_handle->port_parameter.memory_select);
        //HAL_AUDIO_LOG_INFO("singen audio_control %d agent %d\r\n",2,generator_handle->audio_control,agent);
    } else {
        agent = hal_device_convert_agent(generator_handle->audio_control, generator_handle->port_parameter.device_interface);
        //HAL_AUDIO_LOG_INFO("singen audio_control %d device_interface %d rate %d agent %d\r\n",4,generator_handle->audio_control, generator_handle->port_parameter.device_interface,generator_handle->rate,agent);
    }
    hal_sine_generator_set_samplerate(generator_handle->rate);
    hal_sine_generator_set_amplitude(AFE_SINE_GENERATOR_AMPLITUDE_DIVIDE_8);
    hal_sine_generator_set_period(2);
    hal_sine_generator_set_enable(agent, generator_handle->is_input_port, generator_handle->enable);
    return 0;
}

/*******************************************************************************************
*                                     Up/Down Sampler                                      *
********************************************************************************************/
bool hal_audio_updown_set_agent (afe_updown_configuration_t *configure, afe_updown_sampler_id_t updown_id, hal_audio_control_status_t control)
{
    hal_audio_agent_t updown_agent;
    bool is_toggle_control = false;
    if (updown_id == AFE_UPDOWN_SAMPLER_UP_CH01) {
        updown_agent = HAL_AUDIO_AGENT_BLOCK_UP_SAMPLE01;
    } else if (updown_id == AFE_UPDOWN_SAMPLER_DOWN_CH01) {
        updown_agent = HAL_AUDIO_AGENT_BLOCK_DOWN_SAMPLE01;
    } else if (updown_id == AFE_UPDOWN_SAMPLER_DOWN_CH23) {
        if (configure->is_echo_configure_input) {
            updown_agent = HAL_AUDIO_AGENT_BLOCK_DOWN_SAMPLE23_CONFIGURE;
        } else {
            updown_agent = HAL_AUDIO_AGENT_BLOCK_DOWN_SAMPLE23;
        }
    } else {
        HAL_AUDIO_LOG_WARNING("DSP - Warning invalid updown sampler control %d @@", 1, updown_id);
        return true;
    }

    if (control == HAL_AUDIO_CONTROL_ON) {
        hal_audio_agent_user_count[updown_agent]++;
        if (hal_audio_agent_user_count[updown_agent]== 1) {
            if (updown_agent == HAL_AUDIO_AGENT_BLOCK_DOWN_SAMPLE23_CONFIGURE) {
                hal_updown_set_input_rate(updown_id, configure->input_rate);
                hal_tick_align_set_updown(updown_id, configure->tick_align, true);
            } else if (updown_agent == HAL_AUDIO_AGENT_BLOCK_DOWN_SAMPLE23) {
                hal_updown_set_output_rate(updown_id, configure->output_rate);
                hal_updown_set_ratio(updown_id, hal_updown_get_input_rate(updown_id), configure->output_rate);
                hal_audio_updown_set_start(updown_id, control);
            } else {
                hal_audio_updown_set_configuration(updown_id, configure->input_rate, configure->output_rate);
                hal_tick_align_set_updown(updown_id, configure->tick_align, true);
                hal_audio_updown_set_start(updown_id, control);
            }
            is_toggle_control = true;
        } else if (updown_agent != HAL_AUDIO_AGENT_BLOCK_DOWN_SAMPLE23_CONFIGURE){
            HAL_AUDIO_LOG_WARNING("DSP - Warning updown is occupancy %d @@", 1, updown_id);
        }
    }
    else {
        if (hal_audio_agent_user_count[updown_agent] == 1) {
            if (updown_agent == HAL_AUDIO_AGENT_BLOCK_DOWN_SAMPLE23_CONFIGURE) {
                hal_tick_align_set_updown(updown_id, configure->tick_align, false);
            } else if (updown_agent == HAL_AUDIO_AGENT_BLOCK_DOWN_SAMPLE23) {
                hal_audio_updown_set_start(updown_id, control);
            } else {
                hal_tick_align_set_updown(updown_id, configure->tick_align, false);
                hal_audio_updown_set_start(updown_id, control);
            }
            is_toggle_control = true;
        } else if (hal_audio_agent_user_count[updown_agent] < 0) {
            hal_audio_agent_user_count[updown_agent] = 0;
        }
        hal_audio_agent_user_count[updown_agent]--;
    }
    return is_toggle_control;
}

int32_t hal_audio_updown_get_agent_count (afe_updown_sampler_id_t updown_id)
{
    hal_audio_agent_t updown_agent;

    if (updown_id == AFE_UPDOWN_SAMPLER_UP_CH01) {
        updown_agent = HAL_AUDIO_AGENT_BLOCK_UP_SAMPLE01;
    } else if (updown_id == AFE_UPDOWN_SAMPLER_DOWN_CH01) {
        updown_agent = HAL_AUDIO_AGENT_BLOCK_DOWN_SAMPLE01;
    } else if (updown_id == AFE_UPDOWN_SAMPLER_DOWN_CH23) {
        if (1) {
            updown_agent = HAL_AUDIO_AGENT_BLOCK_DOWN_SAMPLE23_CONFIGURE;
        } else {
            updown_agent = HAL_AUDIO_AGENT_BLOCK_DOWN_SAMPLE23;
        }
    } else {

    }
    return hal_audio_agent_user_count[updown_agent];
}

bool hal_audio_updown_set_configuration(afe_updown_sampler_id_t updown_id, uint32_t input_rate, uint32_t output_rate)
{
    hal_updown_set_input_rate(updown_id, input_rate);
    hal_updown_set_output_rate(updown_id, output_rate);
    hal_updown_set_ratio(updown_id, input_rate, output_rate);
    return false;
}

bool hal_audio_updown_set_start(afe_updown_sampler_id_t updown_id, hal_audio_control_status_t control)
{
    if (control == HAL_AUDIO_CONTROL_ON) {
        hal_updown_set_enable(updown_id, true);
    } else {
        hal_updown_set_enable(updown_id, false);
    }
    return false;
}

/*******************************************************************************************
*                                     UL/DL device                                         *
********************************************************************************************/
void hal_audio_adda_set_enable(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_control_adda_counter++;
        if (afe_control_adda_counter==1) {
            hal_audio_adda_set_enable_register(enable);
        }
    } else {
        afe_control_adda_counter--;
        if (afe_control_adda_counter==0) {
            hal_audio_adda_set_enable_register(enable);
        } else if (afe_control_adda_counter<0) {
            afe_control_adda_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_adda_set_global_bias_enable(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_control_global_bias_counter++;
        if (afe_control_global_bias_counter==1) {
            hal_audio_ana_set_global_bias(enable);
        }
    } else {
        afe_control_global_bias_counter--;
        if (afe_control_global_bias_counter==0) {
            hal_audio_ana_set_global_bias(enable);
        } else if (afe_control_global_bias_counter<0) {
            afe_control_global_bias_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

bool hal_audio_adda_set_ul_clock(hal_audio_agent_t device_agent, bool enable)
{
    //Workaround:Prevent state machine is unfinished
    HAL_AUDIO_DELAY_US(5);

    if (device_agent==HAL_AUDIO_AGENT_DEVICE_ADDA_UL2) {
        hal_audio_clock_enable_adc2(enable);
    } else if (device_agent==HAL_AUDIO_AGENT_DEVICE_ADDA_UL3) {
        hal_audio_clock_enable_adc3(enable);
    } else if (device_agent==HAL_AUDIO_AGENT_DEVICE_ADDA_UL4) {
        hal_audio_clock_enable_adda_anc(enable);
    } else {
        hal_audio_clock_enable_adc(enable);
    }

    //Workaround:Prevent state machine is unfinished
    HAL_AUDIO_DELAY_US(5);

    return false;
}

bool hal_audio_adda_set_ul(hal_audio_agent_t device_agent, hal_audio_ul_iir_t iir_filter, uint32_t samplerate, hal_audio_control_status_t control)
{
    if (control == HAL_AUDIO_CONTROL_ON) {
        afe_samplerate_set_ul_samplerate(device_agent, samplerate);

        hal_audio_adda_set_ul_clock(device_agent, true);

        hal_audio_ul_reset_fifo(device_agent, false);

        hal_audio_ul_set_iir(device_agent, iir_filter, true);
        hal_audio_ul_set_hires(device_agent, true);
        hal_audio_adda_set_enable(true);

        hal_audio_ul_set_enable(device_agent, true);
    } else {
        hal_audio_ul_set_enable(device_agent, false);

        hal_audio_adda_set_enable(false);
        hal_audio_ul_set_hires(device_agent, false);
        hal_audio_ul_set_iir(device_agent, iir_filter, false);

        hal_audio_ul_reset_fifo(device_agent, true);

        hal_audio_adda_set_ul_clock(device_agent, false);
    }
    return false;
}
int isPowerOfTwo(unsigned int n)
{
    return n && (!(n & (n - 1)));
}

int findPosition(unsigned int n)
{
    if (!isPowerOfTwo(n))
        return -1;

    unsigned int i = 1, pos = 1;
    while (!(i & n)) {
        i = i << 1;
        ++pos;
    }

    return pos;
}

bool hal_audio_adda_set_bias_enable(hal_audio_bias_selection_t bias_select, hal_audio_bias_voltage_t *bias_voltage, bool is_low_power,bool bias1_2_with_LDO0, hal_audio_control_status_t control)
{
    hal_audio_bias_selection_t bias_search;
    int16_t index = 0;
    int16_t *cunter_ptr;
    if (control == HAL_AUDIO_CONTROL_ON) {
        hal_audio_ana_set_bias_low_power(bias_select, is_low_power);
    }
    for (bias_search = HAL_AUDIO_BIAS_SELECT_BIAS0 ; bias_search<=HAL_AUDIO_BIAS_SELECT_MAX ; bias_search<<=1) {
        if (bias_search&bias_select) {
             if (bias_search == HAL_AUDIO_BIAS_SELECT_BIAS0) {

                cunter_ptr = &afe_control_bias_counter[0];
            } else if (bias_search == HAL_AUDIO_BIAS_SELECT_BIAS1) {
                cunter_ptr = &afe_control_bias_counter[1];
            } else if(bias_search == HAL_AUDIO_BIAS_SELECT_BIAS2){
                cunter_ptr = &afe_control_bias_counter[2];
            } else if(bias_search == HAL_AUDIO_BIAS_SELECT_BIAS3){
                cunter_ptr = &afe_control_bias_counter[3];
            }else{
                cunter_ptr = &afe_control_bias_counter[4];
            }
        } else {
            continue;
        }

        if (control == HAL_AUDIO_CONTROL_ON) {
            (*cunter_ptr)++;
            if (*cunter_ptr == 1) {
                hal_audio_adda_set_global_bias_enable(true);
                index = findPosition(bias_search);
                if(index == -1){
                    HAL_AUDIO_LOG_ERROR("DSP - Error Hal Audio MIC BIAS Wrong:%d !", 1, bias_search);
                    HAL_AUDIO_ASSERT(false);
                }
                hal_audio_ana_set_bias_configuration(bias_search, bias_voltage[index-1], is_low_power,bias1_2_with_LDO0, true);
                HAL_AUDIO_LOG_INFO("findPosition %d,bias_search 0x%x bias_voltage 0x%x true",3,index,bias_search,bias_voltage[index-1]);
            }
        } else {
            (*cunter_ptr)--;
            if (*cunter_ptr == 0) {
                index = findPosition(bias_search);
                if(index == -1){
                    HAL_AUDIO_LOG_ERROR("DSP - Error Hal Audio MIC BIAS Wrong:%d !", 1, bias_search);
                    HAL_AUDIO_ASSERT(false);
                }
                hal_audio_ana_set_bias_configuration(bias_search, bias_voltage[index-1], is_low_power,bias1_2_with_LDO0, false);
                hal_audio_adda_set_global_bias_enable(false);
                HAL_AUDIO_LOG_INFO("findPosition %d,bias_search 0x%x bias_voltage 0x%x false",3,index,bias_search,bias_voltage[index-1]);
            } else if (*cunter_ptr < 0) {
                *cunter_ptr = 0;
            }
        }
    }
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio bias control, bias0:%d, bias1:%d, bias2:%d, bias3:%d, bias4:%d", 5, afe_control_bias_counter[0], afe_control_bias_counter[1], afe_control_bias_counter[2], afe_control_bias_counter[3], afe_control_bias_counter[4]);
    return false;
}

bool hal_audio_adda_set_dl(hal_audio_agent_t device_agent, uint32_t samplerate, hal_audio_dl_sdm_setting_t sdm_setting, hal_audio_control_status_t control)
{
    UNUSED(device_agent);
    if (control == HAL_AUDIO_CONTROL_ON) {
        afe_samplerate_set_dl_samplerate(samplerate);

        hal_audio_clock_set_dac(true);
        hal_audio_dl_set_fifo_swap((afe_analog_control[AFE_ANALOG_DAC].channel_counter.channel_l!=0)/*false*/);
        hal_audio_dl_set_sdm(sdm_setting, true);
        hal_audio_dl_set_hires(device_agent, true);
        hal_audio_adda_set_enable(true);
        HAL_AUDIO_DELAY_US(125);
        #ifdef AIR_A2DP_NLE_ENABLE
        hal_audio_dl_set_nle_enable(true);
        #endif
        hal_audio_dl_set_src_enable(true);
        if ((hal_volume_get_analog_mode(AFE_HW_ANALOG_GAIN_OUTPUT) == HAL_AUDIO_ANALOG_OUTPUT_CLASSAB) ||
            (hal_volume_get_analog_mode(AFE_HW_ANALOG_GAIN_OUTPUT) == HAL_AUDIO_ANALOG_OUTPUT_CLASSG)) {
            hal_gain_set_analog_output_class_ab(afe_volume_analog_get_target_register_value(AFE_HW_ANALOG_GAIN_OUTPUT));
        }

    } else {
        if ((hal_volume_get_analog_mode(AFE_HW_ANALOG_GAIN_OUTPUT) == HAL_AUDIO_ANALOG_OUTPUT_CLASSAB) ||
            (hal_volume_get_analog_mode(AFE_HW_ANALOG_GAIN_OUTPUT) == HAL_AUDIO_ANALOG_OUTPUT_CLASSG)) {
            hal_gain_set_analog_output_class_ab(AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSAB_NEG_32_DB);
        }
        hal_audio_dl_set_src_enable(false);
        #ifdef AIR_A2DP_NLE_ENABLE
        hal_audio_dl_set_nle_enable(false);
        #endif
        hal_audio_adda_set_enable(false);
        hal_audio_dl_set_hires(device_agent, false);
        hal_audio_dl_set_fifo_swap(false);
        hal_audio_clock_set_dac(false);
    }
    return false;
}


void hal_audio_amp_delay_off_timer_callback(HAL_AUDIO_TIMER_HANDLE xTimer)
{
    UNUSED(xTimer);
    if (amp_control.delay_handle.agent != HAL_AUDIO_AGENT_ERROR) {
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio output delay off time up %d", 1, amp_control.delay_handle.agent);
        hal_audio_device_set_agent(&amp_control.delay_handle.device_parameter, amp_control.delay_handle.device_parameter.common.audio_device, HAL_AUDIO_CONTROL_OFF);
        afe_send_amp_status_ccni(false);
    } else {
        //GPIO on delay time up
        hal_audio_device_set_output_gpio(HAL_AUDIO_CONTROL_ON, true);
    }
    amp_control.delay_handle.agent = HAL_AUDIO_AGENT_ERROR;
}

#ifdef AIR_A2DP_NLE_ENABLE
extern afe_volume_digital_control_t afe_digital_gain[AFE_HW_DIGITAL_GAIN_NUM];
static S32 AFE_NLE_GAIN_THRESHOLD_DB = -54; // must < AFE_NLE_GAIN_LIMIT_DB

bool hal_audio_dl_set_nle(VOID* DestBuf, U16 CopySize, U32 format, SOURCE_TYPE type)
{
    S32 HW_DIGITAL_GAIN_DB = afe_digital_gain[AFE_HW_DIGITAL_GAIN1].index/100;
    U32 gain_threshold_16 = afe_calculate_digital_gain_index((AFE_NLE_GAIN_THRESHOLD_DB - HW_DIGITAL_GAIN_DB)*100, 32767);
    U32 gain_threshold_32 = afe_calculate_digital_gain_index((AFE_NLE_GAIN_THRESHOLD_DB - HW_DIGITAL_GAIN_DB)*100, 2147483647);

    //S16 gain_threshold_16 = (S16)((float)pow(10, ((AFE_NLE_GAIN_THRESHOLD_DB - HW_DIGITAL_GAIN_DB) / 20)) * 32767);      // 10^(DB/20)*2^15
    //S32 gain_threshold_32 = (S32)((float)pow(10, ((AFE_NLE_GAIN_THRESHOLD_DB - HW_DIGITAL_GAIN_DB) / 20)) * 2147483647); // 10^(DB/20)*2^31

    int index = 0;
    U32 data_address = 0;
    S16 data_16 = 0;
    S32 data_32 = 0;
    HAL_AUDIO_LOG_INFO("[SET NLE] HW_DIGITAL_GAIN_DB = %d, gain_threshold_16:%d, gain_threshold_32:%d",3,HW_DIGITAL_GAIN_DB, gain_threshold_16, gain_threshold_32);

    if ((type != SOURCE_TYPE_A2DP) || (hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_ANC] > 0)) {
        hal_audio_dl_set_nle_gain(false);
        nle_sample_count = 0;
        return false;
    }
    if (format == 2) {
        for (index = 0; index < CopySize; index+=2) {
            data_address = (U32)(DestBuf+index);
            data_16 = *(S16*)(data_address);
            if ((U32)abs(data_16) > gain_threshold_16){
                //HAL_AUDIO_LOG_INFO("gain_threshold_16:%d, data_16:%d, data_address:0x%x", 3, data_16, data_16, data_address);
                hal_audio_dl_set_nle_gain(false);
                nle_sample_count = 0;
                return false;
            }
        }
    } else if (format == 4) {
        for (index = 0; index < CopySize; index+=4) {
            data_address = (U32)(DestBuf+index);
            data_32 = *(S32*)(data_address);
            //HAL_AUDIO_LOG_INFO("KY gain_threshold_32:%d, |data_32|0:%d, |data_32|+gain:%d", 3, gain_threshold_32, abs(data_32), abs(data_32)-gain_32);
            if ((U32)abs(data_32) > gain_threshold_32){
                hal_audio_dl_set_nle_gain(false);
                nle_sample_count = 0;
                return false;
            }
        }
    }
    nle_sample_count += (CopySize/format);
    HAL_AUDIO_LOG_INFO("[SET NLE] nle_sample_count:%d, data_16:%d, data_32:%d, data_address:0x%x", 4,nle_sample_count, data_16, data_32, data_address);

    if (nle_sample_count >= 24000) {
        hal_audio_dl_set_nle_gain(true);
        nle_sample_count = 0;
    }
    return false;
}
#endif

/*******************************************************************************************
*                                         SideTone                                         *
********************************************************************************************/
#ifdef AIR_SIDETONE_ENABLE
void hal_audio_sidetone_timer_callback(HAL_AUDIO_TIMER_HANDLE xTimer)
{
    UNUSED(xTimer);
    if ((sidetone_control.target_positive_gain == sidetone_control.current_positive_gain) &&
        (sidetone_control.target_negative_gain == sidetone_control.current_negative_gain)) {

        HAL_AUDIO_LOG_INFO("DSP - Hal Audio sidetone ramp done, positve_gain:%d, negative_gain:%d", 2, sidetone_control.current_positive_gain, sidetone_control.current_negative_gain);
        HAL_AUDIO_TIMER_STOP(sidetone_control.timer_handle);
        sidetone_control.ramp_done = true;
        //if (!hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_SIDETONE]) {
        //issue BTA-7231
        if(hal_audio_device_status[HAL_AUDIO_AGENT_DEVICE_SIDETONE-HAL_AUDIO_AGENT_DEVICE_MIN]==HAL_AUDIO_DEVICE_STATUS_STOP){
            sidetone_control.ramp_down_done = true;
            hal_audio_device_set_sidetone(NULL, HAL_AUDIO_CONTROL_DEVICE_SIDETONE, HAL_AUDIO_CONTROL_OFF);
        }else{
            HAL_AUDIO_LOG_INFO("sidetone cnt:%d !=0,will not call entry",1,hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_SIDETONE]);
        }
    } else {
        if (sidetone_control.ramp_start_delay) {
            sidetone_control.ramp_start_delay = false;
            HAL_AUDIO_TIMER_START(sidetone_control.timer_handle, HAL_AUDIO_SIDETONE_RAMP_TIMER_MS);
        }

        sidetone_control.current_negative_gain += sidetone_control.gain_step;
        if ((sidetone_control.target_positive_gain == sidetone_control.current_positive_gain) &&
            ((sidetone_control.gain_step>=0 && (sidetone_control.current_negative_gain>=sidetone_control.target_negative_gain)) ||
             (sidetone_control.gain_step< 0 && (sidetone_control.current_negative_gain<=sidetone_control.target_negative_gain)))) {
            sidetone_control.current_positive_gain = sidetone_control.target_positive_gain;
            sidetone_control.current_negative_gain = sidetone_control.target_negative_gain;
        } else if (sidetone_control.current_negative_gain > AFE_SIDETONE_0DB_REGISTER_VALUE){
            sidetone_control.current_positive_gain += 1;
            sidetone_control.current_negative_gain = 0;
        } else if(sidetone_control.current_negative_gain < 0){
            sidetone_control.current_positive_gain -= 1;
            sidetone_control.current_negative_gain = AFE_SIDETONE_0DB_REGISTER_VALUE;
        }
        hal_sidetone_set_gain_by_register_value(sidetone_control.current_positive_gain, sidetone_control.current_negative_gain);
    }


}
#endif
/*******************************************************************************************
*                                           VAD                                            *
********************************************************************************************/
void hal_audio_vad_delay_timer_callback(HAL_AUDIO_TIMER_HANDLE xTimer)
{
    UNUSED(xTimer);
    hal_audio_ana_set_vad_irq_mask(false);
}
/*******************************************************************************************
*                                         vow                                        *
********************************************************************************************/

void hal_audio_vow_timer_callback(HAL_AUDIO_TIMER_HANDLE xTimer){
    UNUSED(xTimer);
    vow_control.is_stable = false;
    vow_control.is_stable = hal_wow_get_signal_noise_status(&vow_control);
    #if 1
    if(vow_control.is_stable){
        HAL_AUDIO_LOG_INFO("VOW noise is stable to stop timer",0);
        HAL_AUDIO_TIMER_STOP(vow_control.timer_handle);
        hal_wow_clear_snr_irq_status(vow_control);
        vow_control.first_snr_irq = false;
        //NVIC_ClearPendingIRQ(VOW_SNR_IRQn);
        xthal_set_intclear( 1 << VOW_SNR_IRQn);
        hal_nvic_enable_irq(VOW_SNR_IRQn);
    }else{
         HAL_AUDIO_LOG_INFO("VOW noise is not stable 0x%x",1,vow_control.stable_noise);

    }
    #endif
}

#if (HAL_AUDIO_VOW_DEBUG)
void hal_audio_vow_degug_timer_callback(HAL_AUDIO_TIMER_HANDLE xTimer){
    UNUSED(xTimer);
    HAL_AUDIO_LOG_INFO("hal_audio_vow_degug_timer_callback",0);
    hal_wow_get_signal_noise_status(&vow_control);
}
#endif


/*******************************************************************************************
*                                         volume                                         *
********************************************************************************************/
bool hal_audio_volume_set_digital_gain_setting(hal_audio_volume_digital_gain_setting_parameter_t *gain_setting)
{
    afe_hardware_digital_gain_t gain_select = hal_audio_hardware_gain_get_selcet(gain_setting->memory_select);
    afe_volume_digital_set_compensation(gain_select, gain_setting->index_compensation);
    afe_volume_digital_set_ramp_step(gain_select, gain_setting->sample_per_step);
    HAL_AUDIO_LOG_INFO("gain_select %d, index_compensation 0x%x, sample_per_step 0x%x",3,gain_select,gain_setting->index_compensation,gain_setting->sample_per_step);
    return false;
}

#if (HAL_AUDIO_HWGAIN_SET_FADE_TIME)
extern afe_volume_digital_control_t afe_digital_gain[AFE_HW_DIGITAL_GAIN_NUM];

bool hal_audio_volume_set_digital_gain_fade_time_setting(hal_audio_volume_digital_gain_fade_time_setting_parameter_t *gain_fade_tiem_setting)
{
    afe_hardware_digital_gain_t gain_select = hal_audio_hardware_gain_get_selcet(gain_fade_tiem_setting->memory_select);
    uint32_t sample_per_step = 1;
    uint32_t down_step = 0;
    uint32_t up_step = 0;
    uint32_t sample_rate = 0;
    uint32_t current_target_gain = 0;
    int32_t current_gain;
    uint32_t fade_time = 0;
    uint32_t register_value = 0;
    int32_t gain_index = 0;
    int32_t gain_step = 0;
    float db = 0;
    uint32_t RG_VAL = 0;
    UNUSED(RG_VAL);
    UNUSED(register_value);
    hal_audio_volume_digital_gain_parameter_t           digital_gain;
    fade_time = gain_fade_tiem_setting->fade_time;
    gain_index = gain_fade_tiem_setting->gain_index;

    current_target_gain = hal_hw_gain_get_target(gain_select);
    current_gain = hal_hw_gain_get_current_gain(gain_select);
    sample_rate = hal_hw_gain_get_sample_rate(gain_select);
    memset(&digital_gain,0,sizeof(hal_audio_volume_digital_gain_parameter_t));


    HAL_AUDIO_LOG_INFO("Hw Gain fade, hal_audio_volume_set_digital_gain_fade_time_setting(): HWGain%d, current_target_gain:0x%x, current_gain:0x%x, sample_rate:%d, agent_count:%d",
        5, gain_select + 1, current_target_gain, current_gain, sample_rate, hal_audio_hardware_gain_get_agent_count(gain_select));
    if((current_target_gain !=0xFFFFFFFF) && (current_gain!=0xFFFFFFFF)){
        if(1){//if( (!hal_hw_gain_is_running(AFE_HW_DIGITAL_GAIN1)) && (!hal_hw_gain_is_running(AFE_HW_DIGITAL_GAIN2)) && (!hal_hw_gain_is_running(AFE_HW_DIGITAL_GAIN3))){
            //Force target gain to be equal to current
            hal_hw_gain_set_target(gain_select,current_gain);

            switch(gain_select){
                case AFE_HW_DIGITAL_GAIN1:
                    current_gain = afe_digital_gain[gain_select].index;
                    HAL_AUDIO_LOG_INFO("DSP - Hal Audio HW Gain gain select:%d,gain:0x%x",2,gain_select,current_gain);
                break;
                case AFE_HW_DIGITAL_GAIN2:
                    current_gain = afe_digital_gain[gain_select].index;
                    HAL_AUDIO_LOG_INFO("DSP - Hal Audio HW Gain gain select:%d,gain:0x%x",2,gain_select,current_gain);
                break;
                case AFE_HW_DIGITAL_GAIN3:
                    current_gain = afe_digital_gain[gain_select].index;
                    HAL_AUDIO_LOG_INFO("DSP - Hal Audio HW Gain gain select:%d,gain:0x%x",2,gain_select,current_gain);
                break;
            default:
                HAL_AUDIO_LOG_INFO("DSP - Hal Audio HW Gain gain select wrong",0);
            break;
            }
            HAL_AUDIO_LOG_INFO("DSP - HWGAIN gain_index %d,current_gain %d,sample_rate %d,fade_time %d\r\n",4,gain_index,current_gain,sample_rate,fade_time);


            float gain_mute_to_max_db = 0.0f, gain_max_to_mute_db = 0.0f;
            float down_step_in_fp = 0.0f, up_step_in_fp = 0.0f;
            float up_step_in_db = 0.0f, down_step_in_db = 0.0f;
            float fade_time_in_sec = 0.0f;
            float sample_rate_in_fp = 0.0f;
            uint32_t gain_con0_addr = 0;
            uint32_t samples_per_gain_step = 1; // make it short as possible

            switch (gain_select) {
            case AFE_HW_DIGITAL_GAIN1:
                gain_con0_addr = AFE_GAIN1_CON0;
                break;
            case AFE_HW_DIGITAL_GAIN2:
                gain_con0_addr = AFE_GAIN2_CON0;
                break;
            case AFE_HW_DIGITAL_GAIN3:
                gain_con0_addr = AFE_GAIN3_CON0;
                break;
            default:
                break;
            }

            if (AFE_READ(gain_con0_addr) & AFE_GAIN1_CON0_EXTEND_MASK) {
                gain_max_to_mute_db = -84.5f;  // -84.5dB, temporal hard-coding since there is no definition yet
                gain_mute_to_max_db = -gain_max_to_mute_db;
            }
            else {
                gain_max_to_mute_db = -72.5f;  // -72.5dB, temporal hard-coding since there is no definition yet
                gain_mute_to_max_db = -gain_max_to_mute_db;
            }

            // Look up sampling rate in register when specified sampling rate is strange
            if (sample_rate < 8000 || 192000 < sample_rate) {
                uint32_t sample_rate_register_value = ((AFE_READ(gain_con0_addr) >> AFE_GAIN1_CON0_RATE_POS) & 0xF);
                sample_rate_in_fp = (float)afe_samplerate_convert_register_value_to_samplerate(sample_rate_register_value);
            }
            else {
                sample_rate_in_fp = sample_rate;
            }

            // Special case for 0ms, set fade envelope steep as possible
            if (fade_time == 0) {
                up_step = AFE_HW_DIGITAL_GAIN_0DB_REGISTER_VALUE * 2 - 1;
                up_step_in_db = 20.0f * log10f((float)up_step / (float)AFE_HW_DIGITAL_GAIN_0DB_REGISTER_VALUE);
                down_step_in_db = -up_step_in_db;
                down_step_in_fp = powf(10.0f, (down_step_in_db/20.0f)) * (float)AFE_HW_DIGITAL_GAIN_0DB_REGISTER_VALUE;
                down_step = (uint32_t)down_step_in_fp;
            }
            else {
                // Clip fade time range
                if (fade_time > 10000) { // 10sec
                    fade_time = 10000;
                }
                fade_time_in_sec = (float)fade_time / 1000.0f;

                up_step_in_db = gain_mute_to_max_db / (fade_time_in_sec / ((float)samples_per_gain_step / sample_rate_in_fp));
                if (up_step_in_db <= 0.2f) { // make up_step higher than 0.2dB to avoid calculation resolution issue
                    samples_per_gain_step = (uint32_t)((float)samples_per_gain_step * ceilf(0.2f / up_step_in_db));
                    if (samples_per_gain_step > 0xFF) {
                        samples_per_gain_step = 0xFF;
                    }
                    up_step_in_db = gain_mute_to_max_db / (fade_time_in_sec / ((float)samples_per_gain_step / sample_rate_in_fp));
                }
                down_step_in_db = -up_step_in_db;

                up_step_in_fp = powf(10.0f, (up_step_in_db/20.0f)) * (float)AFE_HW_DIGITAL_GAIN_0DB_REGISTER_VALUE;
                down_step_in_fp = powf(10.0f, (down_step_in_db/20.0f)) * (float)AFE_HW_DIGITAL_GAIN_0DB_REGISTER_VALUE;
                up_step = (uint32_t)up_step_in_fp;
                down_step = (uint32_t)down_step_in_fp;

                // assertion
                if (down_step == 0) { // it is a bug if this condition becomes true
                    down_step = 1;
                }
                else if (down_step > AFE_HW_DIGITAL_GAIN_0DB_REGISTER_VALUE - 1)
                {
                    down_step = AFE_HW_DIGITAL_GAIN_0DB_REGISTER_VALUE - 1;
                }
                if (up_step > AFE_HW_DIGITAL_GAIN_0DB_REGISTER_VALUE * 2 - 1) { // 6dB is max due to register bits limit.
                    up_step = AFE_HW_DIGITAL_GAIN_0DB_REGISTER_VALUE * 2 - 1;
                }
                else if (up_step <= AFE_HW_DIGITAL_GAIN_0DB_REGISTER_VALUE)
                {
                    up_step = AFE_HW_DIGITAL_GAIN_0DB_REGISTER_VALUE + 1;
                }
            }

            // for later log function
            sample_per_step = samples_per_gain_step;

            // Write register directly to update samples_per_step
            // Though there is hal_hw_gain_set_sample_per_step() in hal_audio_driver.c,
            // it does not update value when target hw gain is disabled
            AFE_SET_REG(gain_con0_addr, samples_per_gain_step << AFE_GAIN1_CON0_PER_STEP_POS, AFE_GAIN1_CON0_PER_STEP_MASK);

            HAL_AUDIO_LOG_INFO("HW Gain fade step: down_step %d*0.001dB, down_step_val %d, gain_range %d*0.1dB, samples_per_cycle %d, sample_rate %dHz->%dHz, fade_time %dms\r\n",
                7,
                (uint32_t)(1000.0f * down_step_in_db), down_step, (uint32_t)(10.0f * gain_max_to_mute_db),
                samples_per_gain_step, sample_rate, (uint32_t)sample_rate_in_fp, fade_time);
            hal_hw_gain_set_down_step(gain_select, down_step);

            HAL_AUDIO_LOG_INFO("HW Gain fade step: up_step %d*0.001dB, up_step_val %d, gain_range %d*0.1dB, samples_per_cycle %d ,sample_rate %dHz->%dHz, fade_time %dms\r\n",
                7,
                (uint32_t)(1000.0f * up_step_in_db), up_step, (uint32_t)(10.0f * gain_mute_to_max_db),
                samples_per_gain_step, sample_rate, (uint32_t)sample_rate_in_fp, fade_time);
            hal_hw_gain_set_up_step(gain_select, up_step);
            hal_hw_gain_set_target(gain_select, current_target_gain); // restore target gain


            HAL_AUDIO_LOG_INFO("DSP - HWGAIN %d fade time %d,target gain 0x%x,current gain 0x%x",4,gain_select,fade_time,gain_index,current_gain);
            HAL_AUDIO_LOG_INFO("DSP - HWGAIN %d fs %d,sample_per_step %d,down_step 0x%x,up_step 0x%x",5,gain_select,sample_rate,sample_per_step,down_step,up_step);
        }else{
            if(hal_hw_gain_is_running(AFE_HW_DIGITAL_GAIN1))
            HAL_AUDIO_LOG_INFO("DSP - Hal Audio HW Gain1 is running target 0x%x cur 0x%x, can not set fade time",2,hal_hw_gain_get_target(AFE_HW_DIGITAL_GAIN1),hal_hw_gain_get_current_gain(AFE_HW_DIGITAL_GAIN1));
            if(hal_hw_gain_is_running(AFE_HW_DIGITAL_GAIN2))
            HAL_AUDIO_LOG_INFO("DSP - Hal Audio HW Gain2 is running target 0x%x cur 0x%x, can not set fade time",2,hal_hw_gain_get_target(AFE_HW_DIGITAL_GAIN2),hal_hw_gain_get_current_gain(AFE_HW_DIGITAL_GAIN2));
            if(hal_hw_gain_is_running(AFE_HW_DIGITAL_GAIN3))
            HAL_AUDIO_LOG_INFO("DSP - Hal Audio HW Gain3 is running target 0x%x cur 0x%x, can not set fade time",2,hal_hw_gain_get_target(AFE_HW_DIGITAL_GAIN3),hal_hw_gain_get_current_gain(AFE_HW_DIGITAL_GAIN3));

        }

    }else{
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio HW Gain gain select wrong %d",1,gain_select);
    }

    return false;
}

#endif
bool hal_audio_volume_set_digital_gain(hal_audio_volume_digital_gain_parameter_t *digital_gain)
{
    afe_hardware_digital_gain_t gain_select = hal_audio_hardware_gain_get_selcet(digital_gain->memory_select);
    if (digital_gain->is_mute_control) {
        afe_volume_digital_set_mute(gain_select, (afe_volume_mute_control_t)digital_gain->mute_control, digital_gain->mute_enable);
    } else if(digital_gain->is_set_by_register) {
        afe_volume_digital_set_gain_by_value(gain_select, digital_gain->value);
    } else {
        HAL_AUDIO_LOG_INFO("set_digital_gain digital_gain->value 0x%x,cast 0x%x",2,digital_gain->value,(int32_t)digital_gain->value);
        afe_volume_digital_set_gain_by_index(gain_select, (int32_t)digital_gain->value);
    }
    return false;
}

bool hal_audio_volume_set_analog_input_gain(hal_audio_volume_analog_input_gain_parameter_t *input_gain)
{
    afe_hardware_analog_gain_t gain_select;
    if (input_gain->device_interface ==  HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2) {
        gain_select = AFE_HW_ANALOG_GAIN_INPUT2;
    } else if (input_gain->device_interface ==  HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3) {
        gain_select = AFE_HW_ANALOG_GAIN_INPUT3;
    } else {
        gain_select = AFE_HW_ANALOG_GAIN_INPUT1;
    }
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio Gain Input Setting:%d, Gain_L:0x%x, Gain_R:0x%x", 3, gain_select, input_gain->value_l, input_gain->value_r);

    if(input_gain->is_set_by_register) {
        afe_volume_analog_set_gain_by_value(gain_select, input_gain->value_l, input_gain->value_r);
    } else {
        afe_volume_analog_set_gain_by_index(gain_select, (int32_t)input_gain->value_l, (int32_t)input_gain->value_r);
    }
    return false;
}

bool hal_audio_volume_set_analog_output_gain(hal_audio_volume_analog_output_gain_parameter_t *output_gain)
{
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio Gain Output Setting, Gain_L:0x%x, Gain_R:0x%x", 2, output_gain->value_l, output_gain->value_r);
    //#ifdef LINE_IN_PURE_FOR_AMIC_CLASS_G_HQA
    //#else
    if(output_gain->is_set_by_register) {
        afe_volume_analog_set_gain_by_value(AFE_HW_ANALOG_GAIN_OUTPUT, output_gain->value_l, output_gain->value_r);
    } else {
        afe_volume_analog_set_gain_by_index(AFE_HW_ANALOG_GAIN_OUTPUT, (int32_t)output_gain->value_l, (int32_t)output_gain->value_r);
    }
    //#endif
    return false;
}

bool hal_audio_volume_set_analog_output_mode(hal_audio_volume_analog_output_mode_parameter_t *output_mode)
{
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio Output Mode Setting, gain_select:0x%x, dac_mode:0x%x", 2, output_mode->gain_select, output_mode->dac_mode);
    hal_volume_set_analog_mode(output_mode->gain_select,output_mode->dac_mode);

    return false;
}

#ifdef AIR_AUDIO_LR_OUT_ANALOG_GAIN_OFFSET_ENABLE
bool hal_audio_volume_set_analog_output_offset_gain(hal_audio_volume_analog_output_gain_parameter_t *output_gain)
{
    afe_volume_analog_set_offset_gain_by_index(AFE_HW_ANALOG_GAIN_OUTPUT, (int32_t)output_gain->value_l, (int32_t)output_gain->value_r);
    return false;
}
#endif


/*******************************************************************************************
*                                           IRQ                                            *
********************************************************************************************/
static hal_audio_irq_entry afe_irq_audiosys_function[AFE_AUDIOSYS_IRQ_NUM];
static hal_audio_irq_entry afe_irq_src1_function;
static hal_audio_irq_entry afe_irq_src2_function;
static hal_audio_irq_entry afe_irq_anc_function;
static hal_audio_irq_entry afe_irq_vad_function;
#ifdef MTK_TDM_ENABLE
static hal_audio_irq_entry afe_irq_i2s_slave_function[AFE_I2S_SLAVE_IRQ_NUM];
#else
static hal_audio_irq_entry afe_irq_i2s_slave_function;
#endif
//static hal_audio_irq_entry afe_irq_vow_snr_function;
static hal_audio_irq_entry afe_irq_vow_fifo_function;


bool g_run_isr;
static void hal_audio_irq_audiosys_active(uint32_t irq_idx)
{
    if (afe_irq_audiosys_function[irq_idx] != NULL) {
        afe_irq_audiosys_function[irq_idx]();
    }
    else {
        //No such handler
    }
}

void hal_audio_irq_audiosys_handler(hal_nvic_irq_t irq)
{
    hal_audio_irq_audiosys_t irq_index;
    uint32_t volatile irq_status;

    UNUSED(irq);
    g_run_isr = true;
    irq_status = AFE_GET_REG(AFE_IRQ_MCU_STATUS) & AFE_AUDIOSYS_IRQ_AVAILABLE_MASK;

    if (irq_status != 0) {
        /*call each IRQ handler function*/
        for (irq_index = 0; irq_index < AFE_AUDIOSYS_IRQ_NUM; irq_index++) {
            if (irq_status & (0x1<<irq_index) & AFE_GET_REG(AFE_IRQ_MCU_CON0)){
                hal_audio_irq_audiosys_active(irq_index);
            }
        }
    }

    if (irq_status&(0x1<<hal_memory_convert_audiosys_irq_number(HAL_AUDIO_AGENT_MEMORY_DL1))) {
        hal_audio_dl_set_classg_monitor();  //SW workaround
    }

    /* clear irq */
    AFE_SET_REG(AFE_IRQ_MCU_CLR, irq_status, AFE_AUDIOSYS_IRQ_AVAILABLE_MASK);
    g_run_isr = false;
    __asm__ __volatile__ ("nop");

}

//#define HAL_AUDIO_TEMP_SRC_DEBUG
#ifdef HAL_AUDIO_TEMP_SRC_DEBUG
#define HAL_AUDIO_TEMP_RECORD_BUF   50
typedef struct {
    uint32_t time;
    uint32_t ird;
    uint32_t iwr;
    uint32_t ord;
    uint32_t owr;

} afe_src_temp_t, *afe_src_temp_p;
afe_src_temp_t hal_audio_temp_record[HAL_AUDIO_TEMP_RECORD_BUF];
uint32_t hal_audio_temp_record_cnt;
#endif

uint32_t sample_counter;
uint32_t src_irq_1st_flag;
uint32_t src_cnt;
void hal_audio_irq_src1_handler(hal_nvic_irq_t irq)
{
    UNUSED(irq);
    uint32_t irq_en_mask;
    uint32_t volatile irq_status;

#ifdef ENABLE_HWSRC_CLKSKEW
    volatile SINK sink = Sink_blks[SINK_TYPE_AUDIO];
    SOURCE_TYPE source_type = sink->transform->source->type;
    afe_block_t *afe_block = &sink->param.audio.AfeBlkControl;
    BUFFER_INFO *buffer_info = &sink->streamBuffer.BufferInfo;
    uint32_t sample_size = clock_skew_asrc_get_input_sample_size();
    uint32_t xppm_framesize = sample_size * sink->param.audio.channel_num * sink->param.audio.format_bytes;
    uint32_t hw_input_current_write_idx = 0;
    uint32_t asrc_sinkwpt_InBuf_wpt_count = 0;
    afe_asrc_compensating_t pcdc_base_step = COMPENSATING_ADD_0_XPPM;
    afe_asrc_compensating_t asrc_step = COMPENSATING_ADD_0_XPPM;
    uint32_t now_asi = 0;
    uint32_t asi_diff = 0;
    uint32_t hw_output_read_idx = 0;
    uint32_t hw_output_current_write_idx = 0;
    uint16_t src_out_data_size;
#if (ASRC_CLCOK_SKEW_DEBUG)
    uint32_t pre_hw_output_write_idx = 0;
    uint32_t output_xppm_framesize = 0;
    static uint8_t toggle_flag = 0,first_time_flag = 0;
    static int32_t accumulate_output_xppm_framesize = 0;
    static uint32_t pretimer0 = 0,timer0 = 0,duration_count = 0;
    if(afe_block->u4asrcIrqParaCleanDone== false){
        hw_output_current_write_idx = 0;
        toggle_flag = 0;
        first_time_flag = 0;
        accumulate_output_xppm_framesize = 0;
        pretimer0 = 0;
        timer0 = 0;
        duration_count = 0;
        afe_block->u4asrcIrqParaCleanDone = true;
        if(source_type == SOURCE_TYPE_A2DP) {
            sample_counter = 0;
        }
    }
#else
    if(afe_block->u4asrcIrqParaCleanDone == false){
        if(source_type == SOURCE_TYPE_A2DP) {
            sample_counter = 0;
            src_irq_1st_flag = 0;
            src_cnt = 0;

        }
        afe_block->u4asrcIrqParaCleanDone = true;
    }
#endif /*ASRC_CLCOK_SKEW_DEBUG*/
#endif /*ENABLE_HWSRC_CLKSKEW*/

    g_run_isr = true;
    irq_status = AFE_GET_REG(ASM_IFR)&ASM_IFR_MASK;
    src_cnt++;
    //DSP_MW_LOG_I("[PT] irq_status:%d, src_irq_cnt:%d", 2, irq_status, src_cnt);

    if (afe_irq_src1_function) {
        afe_irq_src1_function();
    }


    if (hal_audio_agent_user_count[HAL_AUDIO_AGENT_MEMORY_SRC1]) {//(AFE_GET_REG(AFE_SRC_CONT_CON0)&0x1) {
        //Clear interrupt
        AFE_SET_REG(ASM_SMPCNT_CONF, 1<<ASM_SMPCNT_CONF_IRQ_CLEAR_POS, ASM_SMPCNT_CONF_IRQ_CLEAR_MASK); //Clear sample counter

        //reset output offset for continuous mode
        uint32_t output_bass = AFE_GET_REG(ASM_OBUF_SADR);
        uint32_t output_write = AFE_GET_REG(ASM_CH01_OBUF_WRPNT);
        uint32_t output_new_read;
        if ((output_bass + 32) <= output_write) {
            output_new_read = output_write-32;
        } else {
            output_new_read = output_bass + AFE_GET_REG(ASM_OBUF_SIZE);
        }
        AFE_SET_REG(ASM_CH01_OBUF_RDPNT, output_new_read<<ASM_CH01_OBUF_RDPNT_POS, ASM_CH01_OBUF_RDPNT_MASK);
    } else {
        irq_en_mask = (AFE_GET_REG(ASM_IER))&ASM_IFR_MASK;

        //*******************ASRC IRQ HANDLER Begin************************//
        if(hal_audio_agent_user_count[HAL_AUDIO_AGENT_MEMORY_DL1]){
#ifdef ENABLE_HWSRC_CLKSKEW

        if (afe_get_asrc_irq_is_enabled(AFE_MEM_ASRC_1, ASM_IER_IBUF_EMPTY_INTEN_MASK) && (irq_status&ASM_IFR_IBUF_EMPTY_INT_MASK) && (AFE_READ(ASM_CH01_IBUF_WRPNT)== AFE_READ(ASM_CH01_IBUF_RDPNT))) {
            if(src_irq_1st_flag == 0){
               now_asi = sink->transform->source->param.n9_a2dp.predict_asi;
               extern U32 initial_asi;
               asi_diff= (now_asi-initial_asi)&(0x3FFFFFF);
               DSP_MW_LOG_I("[ASRC] initial_asi:0x%x, now_asi:0x%x, asi_diff:0x%x, src_cnt:%d, irq_status:%d", 5, initial_asi, now_asi, asi_diff, src_cnt, irq_status);
               pcdc_asi_count_add(asi_diff);
            }
            hw_input_current_write_idx = AFE_GET_REG(ASM_CH01_IBUF_WRPNT)-AFE_READ(ASM_IBUF_SADR);
            asrc_sinkwpt_InBuf_wpt_count = (buffer_info->WriteOffset >= hw_input_current_write_idx)? (buffer_info->WriteOffset - hw_input_current_write_idx)
                                         : (buffer_info->WriteOffset + buffer_info->length - hw_input_current_write_idx);
#if (ASRC_CLCOK_SKEW_DEBUG)
            pretimer0 = timer0;
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&timer0);
            hal_gpt_get_duration_count(pretimer0,timer0,&duration_count);
            DSP_MW_LOG_W("asrc in wpt %d Rpt %d asrc_sink_InBuf_count %d td %d pt %d t %d",7,(AFE_READ(ASM_CH01_IBUF_WRPNT) -AFE_READ(ASM_IBUF_SADR)),(AFE_READ(ASM_CH01_IBUF_RDPNT) -AFE_READ(ASM_IBUF_SADR)),asrc_sinkwpt_InBuf_wpt_count,duration_count,pretimer0,timer0);
#endif /*ASRC_CLCOK_SKEW_DEBUG*/
            if( asrc_sinkwpt_InBuf_wpt_count >= (xppm_framesize)){
                /* compute total src1 empty time to detect HWSRC OUTBUFFER UNDERFLOW during eSCO */
                if ((pre_src1_empty_gpt_cnt != 0) && (source_type == SOURCE_TYPE_N9SCO)) {
                    uint32_t cur_cnt = 0;
                    uint32_t diff_cnt = 0;
                    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &cur_cnt);
                    hal_gpt_get_duration_count(pre_src1_empty_gpt_cnt, cur_cnt, &diff_cnt);
                    if (diff_cnt > 15000) { // eSCO only have 15ms buffer
                        DSP_MW_LOG_W("src1 irq, HWSRC UNDERFLOW, time %u us", 1, diff_cnt);
                    }
                    pre_src1_empty_gpt_cnt = 0; // reset for next compute
                }
                /* compute end */
                src_irq_1st_flag = 1;
                sample_counter += sample_size;
#if (ASRC_CLCOK_SKEW_DEBUG)
                pre_hw_output_write_idx = hw_output_current_write_idx;
                hw_output_current_write_idx = AFE_READ(ASM_CH01_OBUF_WRPNT)-AFE_GET_REG(ASM_OBUF_SADR);
                output_xppm_framesize = (hw_output_current_write_idx >= pre_hw_output_write_idx)?(hw_output_current_write_idx - pre_hw_output_write_idx)
                                                   :(hw_output_current_write_idx + sink->param.audio.AfeBlkControl.u4asrc_buffer_size - pre_hw_output_write_idx);
                if(first_time_flag!=0){
                    accumulate_output_xppm_framesize += (output_xppm_framesize-xppm_framesize);
                }
                first_time_flag = 1;
                hw_output_read_idx = AFE_READ(ASM_CH01_OBUF_RDPNT)-AFE_GET_REG(ASM_OBUF_SADR);
                DSP_MW_LOG_W("asrc out prewpt %d wpt %d Rpt %d dl_ro %d output_xppm %d outbuflen %d cp %d",7,pre_hw_output_write_idx,hw_output_current_write_idx,hw_output_read_idx,(AFE_GET_REG(AFE_DL1_CUR)-AFE_GET_REG(AFE_DL1_BASE)),output_xppm_framesize,sink->param.audio.AfeBlkControl.u4asrc_buffer_size,accumulate_output_xppm_framesize);
#else /*ASRC_CLCOK_SKEW_DEBUG*/
                UNUSED(hw_output_read_idx);
                UNUSED(hw_output_current_write_idx);
                UNUSED(src_out_data_size);
#endif
/*** PCDC ASI cnt and drift apply ***/
                if(source_type == SOURCE_TYPE_A2DP) {
                    pcdc_asi_threshold_counter(sample_size, sink->param.audio.rate);
                    pcdc_base_step = pcdc_drift_select_hwscr_step();

                    /*set step*/
                    if(((int32_t)afe_block->u4asrcSetCompensatedSamples - (int32_t)COMPENSATING_ADD_0_XPPM) > 0) {
                        asrc_step = pcdc_base_step + (afe_block->u4asrcSetCompensatedSamples-COMPENSATING_ADD_0_XPPM);
                        //DSP_MW_LOG_W("asrc ++, step:%d, pcdcstep:%d, clkstep:%d", 3, asrc_step, pcdc_base_step, afe_block->u4asrcSetCompensatedSamples);
                    } else if (((int32_t)afe_block->u4asrcSetCompensatedSamples - (int32_t)COMPENSATING_ADD_0_XPPM) < 0) {
                        asrc_step = pcdc_base_step + (afe_block->u4asrcSetCompensatedSamples-COMPENSATING_ADD_0_XPPM);
                        //DSP_MW_LOG_W("asrc --, step:%d, pcdcstep:%d, clkstep:%d", 3, asrc_step, pcdc_base_step, afe_block->u4asrcSetCompensatedSamples);
                    } else {
                        asrc_step = pcdc_base_step;
                        //DSP_MW_LOG_W("asrc pcdc, step:%d, pcdcstep:%d, clkstep:%d", 3, asrc_step, pcdc_base_step, afe_block->u4asrcSetCompensatedSamples);
                    }
                }
                #ifdef AIR_BT_CODEC_BLE_ENABLED
                else if (0)
                {

                    hw_output_current_write_idx = AFE_READ(ASM_CH01_OBUF_WRPNT)-AFE_GET_REG(ASM_OBUF_SADR);
                    hw_output_read_idx = AFE_READ(ASM_CH01_OBUF_RDPNT)-AFE_GET_REG(ASM_OBUF_SADR);
                    src_out_data_size = (hw_output_current_write_idx >= hw_output_read_idx) ?  (hw_output_current_write_idx - hw_output_read_idx) : (hw_output_current_write_idx + AFE_GET_REG(ASM_OBUF_SIZE) - hw_output_read_idx);
                    if ((src_cnt%1000) < 4)
                    {
                        DSP_MW_LOG_W("[DEBUG]src1 irq,BLE audio DL HWSRC UNDERFLOW debug owo:%d oro:%d size:%d thd:%d", 4,hw_output_current_write_idx,hw_output_read_idx,src_out_data_size,(xppm_framesize*sink->param.audio.rate/sink->param.audio.src_rate));
                    }
                    if (sink->transform->source->param.n9ble.underrun_recovery_size == 0)
                    {
                       if (src_out_data_size > (xppm_framesize*sink->param.audio.rate/sink->param.audio.src_rate)*6/5)
                        {
                            DSP_MW_LOG_W("src1 irq,BLE audio DL HWSRC UNDERFLOW detected owo:%d oro:%d", 2,hw_output_current_write_idx,hw_output_read_idx);
                            hw_output_read_idx += 128;
                            hw_output_read_idx %= AFE_GET_REG(ASM_OBUF_SIZE);
                            while (AFE_READ(ASM_CH01_OBUF_WRPNT) != hw_output_read_idx + AFE_GET_REG(ASM_OBUF_SADR))
                            {
                                AFE_WRITE(ASM_CH01_OBUF_WRPNT,hw_output_read_idx + AFE_GET_REG(ASM_OBUF_SADR));
                            }
                            sink->transform->source->param.n9ble.underrun_recovery_size = AFE_GET_REG(ASM_OBUF_SIZE) - src_out_data_size + 128;
                            DSP_MW_LOG_W("HWSRC UNDERFLOW detected modify owo:%d recover size:%d", 2,AFE_READ(ASM_CH01_OBUF_WRPNT)-AFE_GET_REG(ASM_OBUF_SADR),sink->transform->source->param.n9ble.underrun_recovery_size);
                            src_cnt = 0;
                        }
                    }
                    else
                    {
                        U16 recover_size = MIN(sink->transform->source->param.n9ble.underrun_recovery_size,128);
                        recover_size = MIN(recover_size,src_out_data_size);
                        hw_output_current_write_idx += (AFE_GET_REG(ASM_OBUF_SIZE) - recover_size);
                        hw_output_current_write_idx %= AFE_GET_REG(ASM_OBUF_SIZE);
                        DSP_MW_LOG_W("HWSRC UNDERFLOW modify owo:%d size:%d", 2,AFE_READ(ASM_CH01_OBUF_WRPNT)-AFE_GET_REG(ASM_OBUF_SADR),recover_size);
                        while (AFE_READ(ASM_CH01_OBUF_WRPNT) != hw_output_current_write_idx + AFE_GET_REG(ASM_OBUF_SADR))
                        {
                            AFE_WRITE(ASM_CH01_OBUF_WRPNT,hw_output_current_write_idx + AFE_GET_REG(ASM_OBUF_SADR));
                        }
                        sink->transform->source->param.n9ble.underrun_recovery_size -= recover_size;
                    }
                    asrc_step = afe_block->u4asrcSetCompensatedSamples;
                }
                #endif
                else
                {
                    asrc_step = afe_block->u4asrcSetCompensatedSamples;
                }

                afe_set_asrc_compensating_sample(AFE_MEM_ASRC_1, sink->param.audio.rate, asrc_step);
                if((asrc_step)<= COMPENSATING_ADD_976_XPPM && (int32_t)(asrc_step)>= (int32_t)COMPENSATING_SUB_976_XPPM){
                    afe_block->u4asrcGetaccumulate_array[asrc_step]++;
                    //DSP_MW_LOG_W("asrc set compensated sample %d, sample_counter:0x%x, decode_asi:0x%x",3,asrc_step, sample_counter, sink->transform->source->param.n9_a2dp.predict_asi);
                }else{
                    DSP_MW_LOG_W("asrc set wrong compensated sample %d",1,asrc_step);
                }

                hw_input_current_write_idx += xppm_framesize;
                hw_input_current_write_idx %= buffer_info->length;
                AFE_WRITE(ASM_CH01_IBUF_WRPNT, hw_input_current_write_idx + AFE_READ(ASM_IBUF_SADR));

            }else{
                /* clear all interrupt flag */
                afe_clear_asrc_irq(0, ASM_IFR_IBUF_EMPTY_INT_MASK);
                afe_set_asrc_irq_enable(AFE_MEM_ASRC_1, false);
                //DSP_MW_LOG_W("asrc clear all interrupt flag irq_is_enabled %d, sample_counter:0x%x, decode_asi:0x%x", 3, afe_get_asrc_irq_is_enabled(AFE_MEM_ASRC_1, ASM_IER_IBUF_EMPTY_INTEN_MASK), sample_counter, sink->transform->source->param.n9_a2dp.predict_asi);
                if ((pre_src1_empty_gpt_cnt == 0) && (source_type == SOURCE_TYPE_N9SCO)) {
                    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &pre_src1_empty_gpt_cnt); // log gpt cnt when the first empty occurs
                }
                //AFE_SET_REG(ASM_IFR, irq_status, irq_status);//ASM_IFR_MASK
            }
        }
#endif /*#ifdef ENABLE_HWSRC_CLKSKEW*/

        }

        //*******************ASRC IRQ HANDLER End************************//
        AFE_SET_REG(ASM_IFR, irq_status, irq_en_mask);/* Clear SRC interrupt flag */
    }

    hal_audio_dl_set_classg_monitor();  //SW workaround

#ifdef HAL_AUDIO_TEMP_SRC_DEBUG
    static uint32_t src_now_time, src_past_time, src_isr_period;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &src_now_time);
    src_isr_period = src_now_time - src_past_time;
    src_past_time = src_now_time;

    //LOG_PRINT_AUDIO("Audio SRC ISR period:%d", 1, src_isr_period);

    hal_audio_temp_record[hal_audio_temp_record_cnt%HAL_AUDIO_TEMP_RECORD_BUF].time = src_isr_period;
    hal_audio_temp_record[hal_audio_temp_record_cnt%HAL_AUDIO_TEMP_RECORD_BUF].ird = AFE_GET_REG(ASM_CH01_IBUF_RDPNT);
    hal_audio_temp_record[hal_audio_temp_record_cnt%HAL_AUDIO_TEMP_RECORD_BUF].iwr = AFE_GET_REG(ASM_CH01_IBUF_WRPNT);
    hal_audio_temp_record[hal_audio_temp_record_cnt%HAL_AUDIO_TEMP_RECORD_BUF].ord = AFE_GET_REG(ASM_CH01_OBUF_RDPNT);
    hal_audio_temp_record[hal_audio_temp_record_cnt%HAL_AUDIO_TEMP_RECORD_BUF].owr = AFE_GET_REG(ASM_CH01_OBUF_WRPNT);
    hal_audio_temp_record_cnt++;

#endif

    g_run_isr = false;
}

void hal_audio_irq_src2_handler(hal_nvic_irq_t irq)
{
    uint32_t irq_en_mask;
    uint32_t volatile irq_status;
    UNUSED(irq);
    g_run_isr = true;
    irq_status = AFE_GET_REG(ASM2_IFR)&ASM_IFR_MASK;

    if (afe_irq_src2_function) {
        afe_irq_src2_function();
    }


    if (hal_audio_agent_user_count[HAL_AUDIO_AGENT_MEMORY_SRC2]) {
        //Clear interrupt
        AFE_SET_REG(ASM2_SMPCNT_CONF, 1<<ASM_SMPCNT_CONF_IRQ_CLEAR_POS, ASM_SMPCNT_CONF_IRQ_CLEAR_MASK); //Clear sample counter

        //reset output offset for comtinuous mode
        uint32_t output_bass = AFE_GET_REG(ASM2_OBUF_SADR);
        uint32_t output_write = AFE_GET_REG(ASM2_CH01_OBUF_WRPNT);
        uint32_t output_new_read;
        if ((output_bass + 32) <= output_write) {
            output_new_read = output_write-32;
        } else {
            output_new_read = output_bass + AFE_GET_REG(ASM2_OBUF_SIZE);
        }
        AFE_SET_REG(ASM2_CH01_OBUF_RDPNT, output_new_read<<ASM_CH01_OBUF_RDPNT_POS, ASM_CH01_OBUF_RDPNT_MASK);

    } else {
        irq_en_mask = (AFE_GET_REG(ASM2_IER))&ASM_IFR_MASK;
        AFE_SET_REG(ASM2_IFR, irq_status, irq_en_mask);/* Clear SRC interrupt flag */
    }

    hal_audio_dl_set_classg_monitor();  //SW workaround

#ifdef HAL_AUDIO_TEMP_SRC_DEBUG
        static uint32_t src_now_time, src_past_time, src_isr_period;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &src_now_time);
        src_isr_period = src_now_time - src_past_time;
        src_past_time = src_now_time;
if (hal_audio_temp_record_cnt < 45) {
        hal_audio_temp_record[hal_audio_temp_record_cnt%HAL_AUDIO_TEMP_RECORD_BUF].time = src_isr_period;
        hal_audio_temp_record[hal_audio_temp_record_cnt%HAL_AUDIO_TEMP_RECORD_BUF].ird = AFE_GET_REG(ASM2_CH01_IBUF_RDPNT)- AFE_GET_REG(ASM2_IBUF_SADR);
        hal_audio_temp_record[hal_audio_temp_record_cnt%HAL_AUDIO_TEMP_RECORD_BUF].iwr = AFE_GET_REG(ASM2_CH01_IBUF_WRPNT)- AFE_GET_REG(ASM2_IBUF_SADR);
        hal_audio_temp_record[hal_audio_temp_record_cnt%HAL_AUDIO_TEMP_RECORD_BUF].ord = AFE_GET_REG(ASM2_CH01_OBUF_RDPNT)- AFE_GET_REG(ASM2_OBUF_SADR);
        hal_audio_temp_record[hal_audio_temp_record_cnt%HAL_AUDIO_TEMP_RECORD_BUF].owr = AFE_GET_REG(ASM2_CH01_OBUF_WRPNT)- AFE_GET_REG(ASM2_OBUF_SADR);
}
        hal_audio_temp_record_cnt++;

#endif

    g_run_isr = false;
}

void hal_audio_irq_anc_handler(hal_nvic_irq_t irq)
{
    UNUSED(irq);
    if (afe_irq_anc_function) {
        afe_irq_anc_function();
    }
}

void hal_audio_irq_vad_handler(hal_nvic_irq_t irq)
{
    UNUSED(irq);
    hal_audio_ana_set_vad_irq_mask(true);
    if (afe_irq_vad_function) {
        afe_irq_vad_function();
    }
}
void hal_audio_irq_i2S_slave_handler(hal_nvic_irq_t irq)
{
    UNUSED(irq);
#ifdef MTK_TDM_ENABLE
    uint32_t volatile dma_int = I2S_DMA_RG_GLB_STA;

    if (dma_int & (DMA_GLBSTA_IT1_MASK | DMA_GLBSTA_IT3_MASK | DMA_GLBSTA_IT5_MASK | DMA_GLBSTA_IT7_MASK
        | DMA_GLBSTA_IT9_MASK | DMA_GLBSTA_IT11_MASK | DMA_GLBSTA_IT13_MASK | DMA_GLBSTA_IT15_MASK)) {
        if (afe_irq_i2s_slave_function[AFE_I2S_SLAVE_IRQ_DL]) {
            afe_irq_i2s_slave_function[AFE_I2S_SLAVE_IRQ_DL]();
        }
    } else if (dma_int & (DMA_GLBSTA_IT2_MASK | DMA_GLBSTA_IT4_MASK | DMA_GLBSTA_IT6_MASK | DMA_GLBSTA_IT8_MASK
        | DMA_GLBSTA_IT10_MASK | DMA_GLBSTA_IT12_MASK | DMA_GLBSTA_IT14_MASK | DMA_GLBSTA_IT16_MASK)){
        if (afe_irq_i2s_slave_function[AFE_I2S_SLAVE_IRQ_UL]) {
            afe_irq_i2s_slave_function[AFE_I2S_SLAVE_IRQ_UL]();
        }
    } else {
        DSP_MW_LOG_I("[SLAVE VDMA] irq error, dma_int=0x%x", 1, dma_int);
    }
#else
    if (afe_irq_i2s_slave_function) {
        afe_irq_i2s_slave_function();
    }
#endif
}

void hal_audio_irq_vow_snr_handler(hal_nvic_irq_t irq)
{
    UNUSED(irq);
    hal_nvic_disable_irq(VOW_SNR_IRQn);
    if(vow_control.first_snr_irq){
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio VOW first snr irq to start timer",0);
        if(vow_control.timer_handle!=NULL){
            //HAL_AUDIO_TIMER_START(vow_control.timer_handle, HAL_AUDIO_VOW_STABLE_TIMER_MS);
            BaseType_t xHigherPriorityTaskWoken;
            xTimerChangePeriodFromISR(vow_control.timer_handle, pdMS_TO_TICKS(HAL_AUDIO_VOW_STABLE_TIMER_MS), &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }else{
            HAL_AUDIO_LOG_INFO("DSP - Hal Audio VOW timer NULL",0);
        }

    }else{
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio VOW snr irq %d occured ",1,irq);

        uint32_t ch0_signal, ch0_noise = 0xFFFFF, ch1_signal, ch1_noise= 0xFFFFF;
        ch0_signal = (AFE_READ(AFE_VOW_VAD_MON4)&0xFFFF) | (AFE_READ(AFE_VOW_VAD_MON6)<<16);
        ch1_signal = (AFE_READ(AFE_VOW_VAD_MON5)&0xFFFF) | (AFE_READ(AFE_VOW_VAD_MON7)<<16);
        ch1_noise =  (AFE_READ(AFE_VOW_VAD_MON9)&0xFFFF) | (AFE_READ(AFE_VOW_VAD_MON11)<<16);
        ch0_noise =  (vow_control.vow_mode != 3) ? (AFE_READ(AFE_VOW_VAD_MON8)&0xFFFF) | (AFE_READ(AFE_VOW_VAD_MON10)<<16) : (ch1_noise);
        HAL_AUDIO_LOG_INFO("ch0_signal:0x%x, ch0_noise:0x%x , ch1_signal:0x%x, ch1_noise:0x%x, Observed_noise:0x%x \n",5, ch0_signal, ch0_noise, ch1_signal, ch1_noise, vow_control.stable_noise);
        #if 0
        hal_wow_clear_snr_irq_status(vow_control);
        xthal_set_intclear( 1 << VOW_SNR_IRQn);
        hal_nvic_enable_irq(VOW_SNR_IRQn);
        #else
        //save previous noise value
        vow_pre_ch0_noise_msb = (AFE_READ(AFE_VOW_VAD_MON10)& 0x7FFF);
        vow_pre_ch1_noise_msb = (AFE_READ(AFE_VOW_VAD_MON11)& 0x7FFF);
        HAL_AUDIO_LOG_INFO("save vow_pre_ch0_noise_msb:0x%x,vow_pre_ch1_noise_msb:0x%x",2,vow_pre_ch0_noise_msb,vow_pre_ch1_noise_msb);

        hal_nvic_disable_irq(VOW_SNR_IRQn);
        #endif
        if (afe_irq_vow_snr_function) {
            afe_irq_vow_snr_function();
        }

    }

}

uint32_t past_fifo_time, fifo_irq_period, fifo_process_cnt,vow_fifo_cnt;

void hal_audio_irq_vow_fifo_handler(hal_nvic_irq_t irq)
{
    UNUSED(irq);
    if (afe_irq_vow_fifo_function) {
        afe_irq_vow_fifo_function();
    }
        //Afe_CallBackHandler callback;
    uint32_t now_time;
    int32_t *ptr = (int32_t *)VOW_SRAM_BASE;
    uint32_t *ptr_dl = (uint32_t *)vow_control.u4AFE_MEMIF_BUF_WP;
    int32_t copy_size = VOW_SRAM_COPY_SIZE<<2;//Stereo 16-bit
    int32_t i = 0;
    uint32_t hw_current_read_idx = 0;
    uint32_t dl_base_addr = 0;
    uint32_t pre_offset = 0,ReadOffset = 0,WriteOffset = 0;

    UNUSED(ptr_dl);
    UNUSED(i);
if(vow_control.u4AFE_MEMIF_BUF_WP){
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &now_time);
        if (AFE_READ(AFE_VOW_TOP_MON0)&0x2000) {
            HAL_AUDIO_LOG_INFO("FIFO ERROR 0x%x \r\n",1, AFE_READ(AFE_VOW_TOP_MON0));
            AFE_WRITE(AFE_VOW_INTR_CLR, 1<<4);
        }

        //for(i=200;i<250;i++){
        //    HAL_AUDIO_LOG_INFO("ptr[%d]=0x%x ptr_dl=0x%x\r\n",3,i,ptr[i],ptr_dl[i]);
        //}

        //HAL_AUDIO_LOG_INFO("fifo_irq_period %d\r\n",1,fifo_irq_period);

#if 0
            memcpy((void *)vow_control.u4AFE_MEMIF_BUF_WP , (void *)ptr, copy_size);
            vow_control.u4AFE_MEMIF_BUF_WP += copy_size;
            if(vow_control.u4AFE_MEMIF_BUF_WP >= vow_control.u4AFE_MEMIF_BUF_END) {
                vow_control.u4AFE_MEMIF_BUF_WP = vow_control.u4AFE_MEMIF_BUF_BASE;
                 //HAL_AUDIO_LOG_INFO("=== FIFO interrupt wrap === AFE_VOW_TOP_CON5 0x%x fifo_irq_period %d\r\n",2,AFE_READ(AFE_VOW_TOP_CON5),fifo_irq_period);
            } else {

            }
#else
            dl_base_addr = AFE_GET_REG(AFE_DL1_BASE);
            hw_current_read_idx = AFE_GET_REG(AFE_DL1_CUR);
            pre_offset = vow_control.u4AFE_MEMIF_BUF_RP - dl_base_addr;
            ReadOffset  = hw_current_read_idx - dl_base_addr;
            WriteOffset = vow_control.u4AFE_MEMIF_BUF_WP - vow_control.u4AFE_MEMIF_BUF_BASE;
            vow_control.u4AFE_MEMIF_BUF_RP = hw_current_read_idx;
            if(OFFSET_OVERFLOW_CHK_HAL(pre_offset, ReadOffset, WriteOffset)){

                //DSP_MW_LOG_W("SRAM Empty period:%d pR:%d R:%d W:%d", 4,fifo_irq_period, pre_offset, ReadOffset, WriteOffset);
            }
            DSP_D2C_BufferCopy((uint32_t*)vow_control.u4AFE_MEMIF_BUF_WP,
                                (uint32_t*)ptr,
                                       copy_size,
                                       (uint32_t*)vow_control.u4AFE_MEMIF_BUF_BASE,
                                       (vow_control.u4AFE_MEMIF_BUF_END-vow_control.u4AFE_MEMIF_BUF_BASE+1));
            WriteOffset += copy_size;
            WriteOffset %= (vow_control.u4AFE_MEMIF_BUF_END-vow_control.u4AFE_MEMIF_BUF_BASE+1);
            vow_control.u4AFE_MEMIF_BUF_WP = WriteOffset + vow_control.u4AFE_MEMIF_BUF_BASE;
#endif

        fifo_irq_period = now_time - past_fifo_time;
        past_fifo_time = now_time;
        if (vow_fifo_cnt >1 ){
            if ((fifo_irq_period > ((VOW_SRAM_COPY_SIZE>>4)*1000)+100 ) ||
                (fifo_irq_period < ((VOW_SRAM_COPY_SIZE>>4)*1000)-100 )) {

                HAL_AUDIO_LOG_INFO("[[[[[  %s  ]]]]] FIFO interrupt %d period wrong : %d \r\n",3, __FUNCTION__, vow_fifo_cnt, fifo_irq_period);
            }
        }
        vow_fifo_cnt++;
    }
    AFE_WRITE(AFE_VOW_INTR_CLR, 1<<8);//AFE_SET_REG(AFE_VOW_INTR_CLR, 1<<8, 1<<8);
}

void hal_audio_irq_register(hal_audio_irq_parameter_t *irq_parameter)
{
    hal_audio_irq_audiosys_t audiosys_irq;

    switch (irq_parameter->audio_irq) {
        case HAL_AUDIO_IRQ_AUDIOSYS:
            if (irq_parameter->memory_select & HAL_AUDIO_MEMORY_POWER_DETECTOR_MASK) {
                audiosys_irq = (irq_parameter->memory_select & HAL_AUDIO_MEMORY_POWER_DETECTOR_L) ? AFE_AUDIOSYS_IRQ9 : AFE_AUDIOSYS_IRQ14;
            } else {
                audiosys_irq = hal_memory_convert_audiosys_irq_number(hal_memory_convert_agent(irq_parameter->memory_select));
            }
            if (audiosys_irq <AFE_AUDIOSYS_IRQ_NUM) {
                afe_irq_audiosys_function[audiosys_irq] = irq_parameter->entry;
            } else if (irq_parameter->memory_select == HAL_AUDIO_MEMORY_DL_SRC1) {
                goto hal_audio_irq_src1_register;
            } else if (irq_parameter->memory_select == HAL_AUDIO_MEMORY_DL_SRC2) {
                goto hal_audio_irq_src2_register;
            }
            break;
        case HAL_AUDIO_IRQ_SRC1:
            hal_audio_irq_src1_register:
            afe_irq_src1_function = irq_parameter->entry;
            break;
        case HAL_AUDIO_IRQ_SRC2:
            hal_audio_irq_src2_register:
            afe_irq_src2_function = irq_parameter->entry;
            break;
        case HAL_AUDIO_IRQ_ANC:
            afe_irq_anc_function = irq_parameter->entry;
            break;
        case HAL_AUDIO_IRQ_VAD:
            afe_irq_vad_function = irq_parameter->entry;
            break;
        case HAL_AUDIO_IRQ_I2S_SLAVE:
#ifdef MTK_TDM_ENABLE
            if ((irq_parameter->memory_select == HAL_AUDIO_MEMORY_DL_SLAVE_DMA) || (irq_parameter->memory_select == HAL_AUDIO_MEMORY_DL_SLAVE_TDM)) {
                afe_irq_i2s_slave_function[AFE_I2S_SLAVE_IRQ_DL] = irq_parameter->entry;
            } else if ((irq_parameter->memory_select == HAL_AUDIO_MEMORY_UL_SLAVE_DMA) || (irq_parameter->memory_select == HAL_AUDIO_MEMORY_UL_SLAVE_TDM)){
                afe_irq_i2s_slave_function[AFE_I2S_SLAVE_IRQ_UL] = irq_parameter->entry;
            }
#else
            afe_irq_i2s_slave_function = irq_parameter->entry;
#endif
            break;
        case HAL_AUDIO_VOW_SNR:
            afe_irq_vow_snr_function = irq_parameter->entry;
            break;
        case HAL_AUDIO_VOW_FIFO:
            afe_irq_vow_fifo_function = irq_parameter->entry;
        break;
        default:

            break;

    }
}


void hal_audio_irq_initialize(void)
{
    int32_t ret = 0, i;

    //Clear IRQ handler function
    for (i=0 ; i<AFE_AUDIOSYS_IRQ_NUM; i++) {
        afe_irq_audiosys_function[i] = NULL;
    }
    afe_irq_src1_function = NULL;
    afe_irq_src2_function = NULL;
    afe_irq_anc_function = NULL;
    afe_irq_vad_function = NULL;
#ifdef MTK_TDM_ENABLE
    for (i=0 ; i<AFE_I2S_SLAVE_IRQ_NUM; i++) {
        afe_irq_i2s_slave_function[i] = NULL;
    }
#else
    afe_irq_i2s_slave_function = NULL;
#endif
    afe_irq_vow_snr_function = NULL;
    afe_irq_vow_fifo_function = NULL;

    //AUDIOSYS
    hal_nvic_disable_irq(AUDIOSYS0_IRQn);
    ret |= hal_nvic_register_isr_handler(AUDIOSYS0_IRQn, (hal_nvic_isr_t)hal_audio_irq_audiosys_handler);
    ret |= hal_nvic_enable_irq(AUDIOSYS0_IRQn);

    //SRC 1
    hal_nvic_disable_irq(AUDIOSYS1_IRQn);
    ret |= hal_nvic_register_isr_handler(AUDIOSYS1_IRQn, (hal_nvic_isr_t)hal_audio_irq_src1_handler);
    #ifndef AIR_I2S_SLAVE_ENABLE
    ret |= hal_nvic_enable_irq(AUDIOSYS1_IRQn);
    #endif
    //SRC 2
    hal_nvic_disable_irq(AUDIOSYS2_IRQn);
    ret |= hal_nvic_register_isr_handler(AUDIOSYS2_IRQn, (hal_nvic_isr_t)hal_audio_irq_src2_handler);
    #ifndef AIR_I2S_SLAVE_ENABLE
    ret |= hal_nvic_enable_irq(AUDIOSYS2_IRQn);
    #endif
    #if 0
    //ANC
    hal_nvic_disable_irq(ANC_IRQn);
    ret |= hal_nvic_register_isr_handler(ANC_IRQn, (hal_nvic_isr_t)hal_audio_irq_anc_handler);
    ret |= hal_nvic_enable_irq(ANC_IRQn);

    //VAD
    hal_nvic_disable_irq(VAD_IRQn);
    ret |= hal_nvic_register_isr_handler(VAD_IRQn, (hal_nvic_isr_t)hal_audio_irq_vad_handler);
    ret |= hal_nvic_enable_irq(VAD_IRQn);
    #endif

    //I2S slave
    hal_nvic_disable_irq(I2S_SLAVE_IRQn);
    ret |= hal_nvic_register_isr_handler(I2S_SLAVE_IRQn, (hal_nvic_isr_t)hal_audio_irq_i2S_slave_handler);
    ret |= hal_nvic_enable_irq(I2S_SLAVE_IRQn);


    //VOW SNR
    hal_nvic_disable_irq(VOW_SNR_IRQn);
    ret |= hal_nvic_register_isr_handler(VOW_SNR_IRQn, (hal_nvic_isr_t)hal_audio_irq_vow_snr_handler);
    ret |= hal_nvic_enable_irq(VOW_SNR_IRQn);
    #if 0
    //VOW FIFO
    hal_nvic_disable_irq(VOW_FIFO_IRQn);
    ret |= hal_nvic_register_isr_handler(VOW_FIFO_IRQn, (hal_nvic_isr_t)hal_audio_irq_vow_fifo_handler);
    ret |= hal_nvic_enable_irq(VOW_FIFO_IRQn);
    #endif

    if (ret) {
    }

}
#endif /*HAL_AUDIO_MODULE_ENABLED*/

