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

#ifndef __DSP_DRV_AFE_CONTROL_H__
#define __DSP_DRV_AFE_CONTROL_H__

#include "hal_audio.h"
#include "types.h"

#ifdef HAL_AUDIO_MODULE_ENABLED

#include "ab156x.h"
#include "hal_nvic.h"
#include "hal_audio_afe_define.h"
#include "hal_audio_afe_connection.h"
#include "hal_audio_control.h"//moidfy for ab1568
#define MASK_ALL       (0xFFFFFFFF)
#define MINIMUM(a,b)   ((a) < (b) ? (a) : (b))
#define AFE_MAX(x,y)    (x > y) ? x : y

/*[Todo]modify name*/
#define AFE_PCM_TRIGGER_STOP          (0)
#define AFE_PCM_TRIGGER_START         (1)
#define AFE_PCM_TRIGGER_PAUSE_PUSH    (3)
#define AFE_PCM_TRIGGER_PAUSE_RELEASE (4)
#define AFE_PCM_TRIGGER_SUSPEND       (5)
#define AFE_PCM_TRIGGER_RESUME        (6)
#define AFE_PCM_TRIGGER_DRAIN         (7)

typedef struct {
    uint32_t phys_buffer_addr;
    //uint8_t  *pSramBufAddr;
    uint32_t u4BufferSize;
    uint32_t u4asrc_buffer_size;/* asrc output buffer size      */
    int32_t  u4DataRemained;
    uint32_t u4SampleNumMask;    /* sample number mask */
    uint32_t u4SamplesPerInt;    /* number of samples to play before interrupting */
    int32_t  u4WriteIdx;         /* Previous Write Index. */
    int32_t  u4ReadIdx;          /* Previous Read Index. */
    uint32_t u4MaxCopySize;
    bool     u4awsflag;          /* Indicate AWS is on or not*/
    bool     u4asrcflag;         /* Indicate ASRC is on or not**/
    uint32_t u4asrcrate;         /* sample rate of ASRC out */
#ifdef ENABLE_HWSRC_CLKSKEW
    bool     u4asrcIrqParaCleanDone; /* Indicates that the ASRC irq parameter has been cleared**/
    uint32_t u4asrcSetCompensatedSamples; /* Indicates that the ASRC irq parameter has been cleared**/
    uint32_t u4asrcGetaccumulate_array[AFE_HWSRC_STEP_NUM];/* Indicates each compensation sample information of the ASRC**/
#endif
} afe_block_t;

typedef struct {
    uint32_t reg;
    uint32_t sbit;
    uint32_t mask;
} audio_register_bit_info_t;

typedef struct  {
    audio_register_bit_info_t on;
    audio_register_bit_info_t mode;
    audio_register_bit_info_t cnt;
    audio_register_bit_info_t clr;
    audio_register_bit_info_t missclr;
    audio_register_bit_info_t status;
    audio_register_bit_info_t en;
    bool   afe_irq_available;
} afe_irq_ctrl_reg_t;

typedef struct {
    int32_t  mFormat;
    int32_t  mDirection;
    uint32_t mSampleRate;
    uint32_t mChannels;
    uint32_t mBufferSize;
    uint32_t mInterruptSample;
    uint32_t mMemoryInterFaceType;
    uint32_t mClockInverse;
    uint32_t mMonoSel;
    uint32_t mdupwrite;
    uint32_t mState;
    uint32_t mFetchFormatPerSample;
    int32_t  mUserCount;
    void     *privatedata;
} afe_audio_memif_attr_t;

typedef struct {
    uint32_t mValid;
    uint32_t mLength;
    uint32_t msram_phys_addr;
    afe_block_t *mUser; //mUser is record for User using substream pointer as reach user
} afe_sram_block_t;

typedef struct {
    uint32_t mSram_phys_addr;
    uint32_t mSramLength;
    uint32_t mBlockSize;
    uint32_t mBlocknum;
    afe_sram_block_t mAud_sram_block[14];  //[18]
    afe_sram_mode_t sram_mode;
} afe_sram_manager_t;

typedef struct {
    hal_audio_device_t               in_device;
    hal_audio_interface_t            in_interface;
    uint32_t                         in_misc_parms;
    hal_audio_device_t               out_device;
    hal_audio_interface_t            out_interface;
    uint32_t                         out_misc_parms;
    hal_audio_channel_selection_t    in_channel;    /*HW out channel default R+L*/
    uint32_t                         gain;
    uint32_t                         sample_rate;
    uint16_t                         *FIR_nvdm_param;
    hal_audio_analog_mdoe_t          adc_mode;
    hal_audio_device_parameter_t     device_handle_in;//modify for ab1568
    hal_audio_device_parameter_t     device_handle_in_side_tone;//modify for ab1568
    hal_audio_device_parameter_t     device_handle_out;//modify for ab1568
} afe_sidetone_param_t, *afe_sidetone_param_p;

typedef struct {
    hal_audio_device_t               in_device;
    hal_audio_interface_t            in_interface;
    afe_misc_parms_t                 in_misc_parms;
    hal_audio_device_t               out_device;
    hal_audio_interface_t            out_interface;
    afe_misc_parms_t                 out_misc_parms;

    hal_audio_channel_selection_t    stream_channel;
    afe_pcm_format_t                 format;
    uint32_t                         sample_rate;
    uint32_t                         with_hw_gain;
    hal_audio_device_parameter_t     device_handle_in;//modify for ab1568
    hal_audio_device_parameter_t     device_handle_out;//modify for ab1568
    hal_audio_path_parameter_t       path_handle;//modify for ab1568
} afe_loopback_param_t, *afe_loopback_param_p;

//---------------------------------------
//  Function at hal_audio_afe_control.c
//---------------------------------------

void afe_control_init(void);

/*interrupt operation*/
int32_t afe_enable_audio_irq(afe_irq_mode_t _irq, uint32_t _rate, uint32_t _count);
int32_t afe_disable_audio_irq(afe_irq_mode_t _irq);
int32_t afe_update_audio_irq_cnt(afe_irq_mode_t _irq, uint32_t _count);
hal_nvic_status_t afe_register_audio_irq(uint32_t irq_number);

/*sram operation*/
uint32_t afe_allocate_audio_sram(afe_block_t *afe_block, afe_pcm_format_t format, uint32_t sram_length, uint32_t force_normal);
bool afe_clear_memory_block(afe_block_t *afe_block, audio_digital_block_t mem_block);
void afe_free_audio_sram(afe_pcm_format_t type);

/*afe hardware control*/
bool afe_get_audio_apll_enable(void);
void afe_audio_apll_enable(bool enable, uint32_t samplerate);
uint32_t afe_get_dac_enable(void);
uint32_t afe_get_i2s0_enable(void);
#if 0//modify for ab1568
uint32_t afe_get_i2s_enable(afe_i2s_num_t i2s_module);
#endif
uint32_t afe_get_all_i2s_enable(void);
void afe_set_dac_enable(bool enable);
void afe_set_dac_in(uint32_t samplerate);
uint32_t afe_get_dac_in_samplerate(void);


void afe_set_adc_in(uint32_t samplerate);
uint32_t afe_get_adc_in_samplerate(void);
uint32_t afe_get_ul_src_addr(hal_audio_interface_t mic_interface);
void afe_set_dmic_samplerate(hal_audio_interface_t dmic_interface, uint32_t samplerate);
uint32_t afe_get_dmic_samplerate(hal_audio_interface_t dmic_interface);

void afe_set_adc_enable(bool enable, hal_audio_interface_t mic_interface);
void afe_set_adda_enable(bool enable);
void afe_set_ul_src_enable(bool enable, hal_audio_interface_t mic_interface);
void afe_set_dl_src_enable(bool enable);
uint32_t afe_get_memory_path_enable(audio_digital_block_t aud_block);
bool afe_set_memory_path_enable(audio_digital_block_t aud_block, bool modify_reg, bool enable);
bool afe_set_memif_fetch_format_per_sample(uint32_t interface_type, uint32_t fetch_format);

/*misc*/
uint32_t afe_samplerate_transform(uint32_t samplerate, audio_digital_block_t aud_block);
uint32_t afe_general_samplerate_transform(uint32_t samplerate);
uint32_t word_size_align(uint32_t in_size);

//---------------------------------------
//  Function at hal_audio_afe_driver.c
//---------------------------------------
/*interrupt operation*/
void audsys_irq_handler(hal_nvic_irq_t irq);
afe_irq_mode_t afe_irq_request_number(audio_digital_block_t mem_block);

/*sram operation*/
afe_sram_mode_t afe_get_prefer_sram_mode(void);
void afe_set_sram_mode(afe_sram_mode_t sram_mode);
void afe_set_mem_block_addr(audio_digital_block_t mem_block, afe_block_t *afe_block);

/*afe hardware control*/
void afe_set_enable(bool enable);
void afe_set_adda_reg(bool enable);
void afe_set_dl_src_reg(bool enable);
bool afe_get_dl_src_reg_status(void);
void afe_set_ul_src_reg(bool enable, hal_audio_interface_t mic_interface);
bool afe_set_dmic_path(bool enable, hal_audio_interface_t dmic_interface, uint32_t sample_rate);
bool afe_set_samplerate(audio_digital_block_t aud_block, uint32_t samplerate);
bool afe_set_channels(audio_digital_block_t aud_block, uint32_t channel);
bool afe_set_memif_format_reg(uint32_t interface_type, uint32_t fetch_format);
bool afe_set_memory_path_enable_reg(audio_digital_block_t aud_block, bool enable);
#if 0//modify for ab1568
void afe_set_i2s_enable(bool enable, afe_i2s_num_t i2s_module, afe_i2s_role_t mode);
void afe_set_i2s_reg(afe_i2s_num_t i2s_module, afe_i2s_role_t mode, afe_i2s_wlen_t data_length, afe_general_samplerate_t rate, afe_i2s_swap_t swap, uint32_t misc_parms);
uint32_t afe_get_i2s_master_samplerate(afe_i2s_num_t i2s_module);
#endif
void afe_set_apll_for_i2s_reg(bool enable, uint32_t samplerate);
#if 0//modify for ab1568
void afe_set_i2s_mclk_reg(bool enable, afe_i2s_num_t i2s_module);
void afe_i2s_enable(bool enable, afe_i2s_num_t i2s_module);
#endif

afe_audio_bt_sync_enable_t afe_get_bt_sync_enable_bit (audio_digital_block_t mem_block);
void afe_bt_sync_enable(bool enable, audio_digital_block_t mem_block);
uint32_t afe_get_bt_sync_monitor(audio_digital_block_t mem_block);
uint32_t afe_get_bt_sync_monitor_state(audio_digital_block_t mem_block);
void afe_26m_xtal_cnt_enable(bool enable);
void afe_dcc_mic_enable(bool enable, hal_audio_device_t audio_device, uint8_t MicbiasSourceType);
#ifndef ENABLE_HWSRC_CLKSKEW
#ifdef AIR_AUDIO_HARDWARE_ENABLE
#ifndef MTK_HWSRC_IN_STREAM
uint32_t afe_calculate_digital_gain_index(int32_t digital_gain_in_01unit_db, uint32_t digital_0db_register_value);
#endif
#endif /* AIR_AUDIO_HARDWARE_ENABLE */

#endif
audio_digital_block_t afe_get_digital_block_by_audio_device (hal_audio_device_t device, hal_audio_interface_t audio_interface, bool is_input);


/*Predistortion*/
void afe_predistortion_switch(void);
void afe_set_predistortion(bool enable);

/*Volume*/
uint32_t afe_audio_get_output_analog_gain_index(void);
uint32_t afe_audio_get_input_analog_gain_index(void);
void afe_audio_set_output_analog_gain_with_ramp(bool ramp_down, uint32_t *value);
void afe_audio_set_output_analog_gain(void);
void afe_audio_set_input_analog_gain(void);
void afe_audio_set_output_digital_gain(afe_digital_gain_t gain_type);
int32_t afe_audio_get_input_digital_gain(afe_input_digital_gain_t index);
void hal_audio_set_gain_parameters(int16_t gain1_recoup, int16_t gain2_recoup, uint16_t gain1_per_step, uint16_t gain2_per_step);

/*HW gain*/
void afe_enable_hardware_digital_gain(afe_hardware_digital_gain_t gain_type, bool enable);
void afe_set_hardware_digital_gain_mode(afe_hardware_digital_gain_t gain_type, uint32_t sample_rate);
void afe_set_hardware_digital_gain(afe_hardware_digital_gain_t gain_type, uint32_t gain);
bool afe_get_hardware_digital_status(afe_hardware_digital_gain_t gain_type);
uint32_t afe_get_hardware_digital_gain(afe_hardware_digital_gain_t gain_type);
void afe_mute_digital_gain(bool mute, afe_digital_gain_t digital_gain);

/*sidetone*/
void afe_sidetone_init(void);
bool afe_set_sidetone_filter(bool enable);
void afe_set_sidetone_input_path(audio_digital_block_t input_block);
void afe_set_sidetone_output_path(afe_sidetone_path_t path, bool enable);
void afe_set_sidetone_volume(int32_t gain);
void afe_set_sidetone_enable(bool enable, afe_sidetone_param_t *param, bool sidetone_rampdown_done_flag);
void afe_set_sidetone_enable_flag(BOOL is_enable, int32_t gain);
EXTERN bool afe_get_sidetone_enable_flag(VOID);
EXTERN int32_t afe_get_sidetone_gain(VOID);
void afe_set_sidetone_input_channel(hal_audio_device_t audio_device);
bool afe_get_sidetone_input_channel(void);

/*Loopback*/
void afe_set_loopback_enable(bool enable, afe_loopback_param_p param);

/*Sinegen*/
#if 0//modify for ab1568
void afe_set_sine_gen_samplerate(afe_general_samplerate_t sample_rate);
#endif
void afe_set_sine_gen_amplitude(afe_sgen_amp_div_t amp_divide);
void afe_sine_gen_enable(uint32_t connection, audio_memif_direction_t direction, bool enable);


/*device */
EXTERN void afe_amp_enable(bool enable, hal_audio_device_t audio_device);
void afe_audio_device_enable(bool enable, hal_audio_device_t audio_device, hal_audio_interface_t audio_interface, afe_pcm_format_t format, uint32_t rate, afe_misc_parms_t misc_parms);
uint32_t afe_get_audio_device_samplerate(hal_audio_device_t audio_device, hal_audio_interface_t audio_interface);
void afe_amp_keep_enable_state(bool enable);

/*misc*/
const afe_irq_ctrl_reg_t *afe_get_irq_control_reg(uint32_t irq_idx);
uint32_t afe_get_sram_phys_addr(void);
uint32_t afe_get_afe_sram_length(void);
void afe_set_lr_swap(bool enable);
void afe_dump_analog_reg(void);
uint32_t afe_reg_value_transform(uint32_t reg, audio_digital_block_t aud_block);

extern bool afe_set_irq_samplerate(afe_irq_mode_t irq_mode, uint32_t samplerate);
extern bool afe_set_irq_counter(afe_irq_mode_t irq_mode, uint32_t counter);

/*asrc*/
void afe_register_asrc_irq_callback_function(hal_asrc_irq_callback_function_t function);
int32_t afe_power_on_asrc(afe_mem_asrc_id_t asrc_id, bool on);
void afe_set_asrc_iir(afe_mem_asrc_id_t asrc_id, uint32_t input_rate, uint32_t output_rate);
void afe_set_asrc_irq_enable(afe_mem_asrc_id_t asrc_id, bool enable);
bool afe_get_asrc_irq_is_enabled(afe_mem_asrc_id_t asrc_id, uint32_t interrupt);
void afe_clear_asrc_irq(afe_mem_asrc_id_t asrc_id, uint32_t status);
uint32_t afe_get_asrc_irq_status(afe_mem_asrc_id_t asrc_id);
void afe_set_asrc(afe_mem_asrc_id_t asrc_id, afe_asrc_config_p config, bool enable);
int32_t afe_mem_asrc_enable(afe_mem_asrc_id_t asrc_id, bool enable);
afe_asrc_tracking_clock_t afe_set_asrc_tracking_clock(hal_audio_interface_t audio_interface);
void afe_set_asrc_enable(bool enable, afe_mem_asrc_id_t asrc_id, afe_asrc_config_p asrc_config);
#ifdef ENABLE_HWSRC_CLKSKEW
void afe_set_asrc_compensating_sample(afe_mem_asrc_id_t asrc_id, uint32_t output_buffer_rate, afe_asrc_compensating_t cp_point);
#endif

#endif //#ifdef HAL_AUDIO_MODULE_ENABLED
#endif /* __DSP_DRV_AFE_CONTROL_H__ */
