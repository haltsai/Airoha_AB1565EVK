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

#ifndef __HAL_AUDIO_CONTROL_H__
#define __HAL_AUDIO_CONTROL_H__

#include "hal_audio.h"
#ifdef HAL_AUDIO_MODULE_ENABLED
#include "hal_audio_path.h"

#if 0

#include "timers.h"
#define HAL_AUDIO_TIMER_HANDLE                                                      TimerHandle_t
#define HAL_AUDIO_TIMER_CREATE(timer_name, time_ms, is_periodic, timer_callback)    xTimerCreate(timer_name, pdMS_TO_TICKS(time_ms), is_periodic, 0, timer_callback)
#define HAL_AUDIO_TIMER_START(timer_handler, time_ms)                               xTimerChangePeriod(timer_handler, pdMS_TO_TICKS(time_ms), 0)
#define HAL_AUDIO_TIMER_STOP(timer_handler)                                         xTimerStop(timer_handler, 0)
#endif
#include "FreeRTOS.h"
#include "timers.h"
#include "semphr.h"

extern bool g_run_isr;
#define HAL_AUDIO_SEMAPHO_HANDLE                                                    SemaphoreHandle_t
#define HAL_AUDIO_TIMER_HANDLE                                                      TimerHandle_t
#define HAL_AUDIO_TIMER_CREATE(timer_name, time_ms, is_periodic, timer_callback)    xTimerCreate(timer_name, pdMS_TO_TICKS(time_ms), is_periodic, 0, timer_callback)
#define HAL_AUDIO_TIMER_START(timer_handler, time_ms)                                                   \
        if (HAL_NVIC_QUERY_EXCEPTION_NUMBER > HAL_NVIC_NOT_EXCEPTION ) {                                                                                \
            BaseType_t xHigherPriorityTaskWoken;                                                        \
            xTimerChangePeriodFromISR(timer_handler, pdMS_TO_TICKS(time_ms), &xHigherPriorityTaskWoken);\
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);                                               \
        } else { xTimerChangePeriod(timer_handler, pdMS_TO_TICKS(time_ms), 0); }
#define HAL_AUDIO_TIMER_STOP(timer_handler)                                                             \
        if (HAL_NVIC_QUERY_EXCEPTION_NUMBER > HAL_NVIC_NOT_EXCEPTION ) {                                                                                \
            BaseType_t xHigherPriorityTaskWoken;                                                        \
            xTimerStopFromISR(timer_handler, &xHigherPriorityTaskWoken);                                \
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);                                               \
        } else { xTimerStop(timer_handler, 0); }
#define OFFSET_OVERFLOW_CHK_HAL(preOffset, nowOffset, comparedOffset) (\
                                        ((preOffset==nowOffset) && (preOffset==comparedOffset))\
                                            ? TRUE\
                                            : (nowOffset>=preOffset)\
                                                ? (comparedOffset<=nowOffset && comparedOffset>preOffset)\
                                                    ? TRUE\
                                                    : FALSE\
                                                : (comparedOffset<=nowOffset || comparedOffset>preOffset)\
                                                    ? TRUE\
                                                    : FALSE)


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*  IRQ */
#define AFE_AUDIOSYS_IRQ_AVAILABLE_MASK (0x5AFF)

/*  SRAM manager */
#define HAL_AUDIO_MEMORY_SRAM_ENABLE          (1)
#define HAL_AUDIO_MEMORY_SRAM_BASE            (0xC0002000)
#define HAL_AUDIO_MEMORY_SRAM_SIZE            (0x10800)
#define HAL_AUDIO_MEMORY_SRAM_BLOCK_SIZE      (0x800)
#define HAL_AUDIO_MEMORY_SRAM_BLOCK_NUMBER    (HAL_AUDIO_MEMORY_SRAM_SIZE/HAL_AUDIO_MEMORY_SRAM_BLOCK_SIZE)

/*  SRC */
#define HAL_AUDIO_SRC_COMPENSATION_MAX_STEP   (32)

/*  Sidetone */
#define HAL_AUDIO_SIDETONE_TIMER_CALLBACK
#define HAL_AUDIO_SIDETONE_MUTE_NEGATIVE_VALUE  (0)    //
#ifdef HAL_AUDIO_SIDETONE_TIMER_CALLBACK
#define HAL_AUDIO_SIDETONE_RAMP_UP_STEP         (200)   //
#define HAL_AUDIO_SIDETONE_RAMP_DOWN_STEP       (1600)   //
#define HAL_AUDIO_SIDETONE_ON_DELAY_TIMER_MS    (150)   // Start to ramp up after X ms
#define HAL_AUDIO_SIDETONE_RAMP_TIMER_MS        (10)    // 10ms
#else
#define HAL_AUDIO_SIDETONE_RAMP_UP_STEP         (16)    //
#define HAL_AUDIO_SIDETONE_RAMP_DOWN_STEP       (32)    //
#define HAL_AUDIO_SIDETONE_RAMP_TIMER_US        (100)   // 100 us
#endif

/*  AMP */
#define HAL_AUDIO_DELAY_OUTPUT_OFF_TIME_MS      (0)
#define HAL_AUDIO_DELAY_GPIO_ON_TIME_MS         (100)

#define HAL_AUDIO_AMP_OUTPUT_GPIO_DISABLE       (0xFF)  //Disable output gpio control macro
#define HAL_AUDIO_AMP_OUTPUT_GPIO               (0xFF)  //Set 0xFF to disable output gpio

/* VOW*/
#define HAL_AUDIO_VOW_STABLE_TIMER_MS           (10)    // 10ms
/*  VAD */
#define HAL_AUDIO_VAD_DELAYON_TIMER_MS          (500)
#define HAL_AUDIO_VAD_DELAYON_TIMER_AFE_MS      (2000)


/*  Feature */
#define HAL_AUDIO_CHANGE_OUTPUT_RATE            (1)
#if (FEA_SUPP_DSP_VAD)
#define HAL_AUDIO_VAD_DRIVER                    (1)
#endif
#define HAL_AUDIO_VOW_DEBUG                     (0)
#define HAL_AUDIO_HWGAIN_SET_FADE_TIME          (0)
#define HAL_AUDIO_KEEP_ADC_HIGHER_PERFORMANCE_MODE   (0)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Type Definitions ///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @brief hal audio device status. */
typedef enum {
    HAL_AUDIO_DEVICE_STATUS_STOP    = 0,
    HAL_AUDIO_DEVICE_STATUS_OPEN,
    HAL_AUDIO_DEVICE_STATUS_PENDING,
} hal_audio_device_status_t;

/** @brief hal audio memory agent. */
typedef enum {
    /* memory interfrace */
    HAL_AUDIO_AGENT_MEMORY_DL_MIN                   = 0,
    HAL_AUDIO_AGENT_MEMORY_DL1                      = HAL_AUDIO_AGENT_MEMORY_DL_MIN,
    HAL_AUDIO_AGENT_MEMORY_DL2                      = 1,
    HAL_AUDIO_AGENT_MEMORY_DL3                      = 2,
    HAL_AUDIO_AGENT_MEMORY_DL12                     = 3,
    HAL_AUDIO_AGENT_MEMORY_DL_MAX                   = HAL_AUDIO_AGENT_MEMORY_DL12,

    HAL_AUDIO_AGENT_MEMORY_UL_MIN                   = 4,
    HAL_AUDIO_AGENT_MEMORY_VUL1                     = HAL_AUDIO_AGENT_MEMORY_UL_MIN,
    HAL_AUDIO_AGENT_MEMORY_VUL2                     = 5,
    HAL_AUDIO_AGENT_MEMORY_VUL3                     = 6,
    HAL_AUDIO_AGENT_MEMORY_AWB                      = 7,
    HAL_AUDIO_AGENT_MEMORY_AWB2                     = 8,
    HAL_AUDIO_AGENT_MEMORY_UL_MAX                   = HAL_AUDIO_AGENT_MEMORY_AWB2,

    HAL_AUDIO_AGENT_MEMORY_SRC_MIN                  = 9,
    HAL_AUDIO_AGENT_MEMORY_SRC1                     = HAL_AUDIO_AGENT_MEMORY_SRC_MIN,
    HAL_AUDIO_AGENT_MEMORY_SRC2                     = 10,
    HAL_AUDIO_AGENT_MEMORY_SRC_MAX                  = HAL_AUDIO_AGENT_MEMORY_SRC2,

    /* block interfrace */
    HAL_AUDIO_AGENT_BLOCK_HWGAIN_MIN                = 11,
    HAL_AUDIO_AGENT_BLOCK_HWGAIN1                   = HAL_AUDIO_AGENT_BLOCK_HWGAIN_MIN,
    HAL_AUDIO_AGENT_BLOCK_HWGAIN2                   = 12,
    HAL_AUDIO_AGENT_BLOCK_HWGAIN3                   = 13,
    HAL_AUDIO_AGENT_BLOCK_HWGAIN_MAX                = HAL_AUDIO_AGENT_BLOCK_HWGAIN3,

    HAL_AUDIO_AGENT_BLOCK_UPDN_MIN                  = 14,
    HAL_AUDIO_AGENT_BLOCK_UP_SAMPLE01               = HAL_AUDIO_AGENT_BLOCK_UPDN_MIN,
    HAL_AUDIO_AGENT_BLOCK_DOWN_SAMPLE01             = 15,
    HAL_AUDIO_AGENT_BLOCK_DOWN_SAMPLE23             = 16,
    HAL_AUDIO_AGENT_BLOCK_DOWN_SAMPLE23_CONFIGURE   = 17,
    HAL_AUDIO_AGENT_BLOCK_UPDN_MAX                  = HAL_AUDIO_AGENT_BLOCK_DOWN_SAMPLE23,

    /* device interfrace */
    HAL_AUDIO_AGENT_DEVICE_MIN                      = 18,
    HAL_AUDIO_AGENT_DEVICE_I2S_SLAVE_MIN            = HAL_AUDIO_AGENT_DEVICE_MIN,
    HAL_AUDIO_AGENT_DEVICE_I2S0_SLAVE               = HAL_AUDIO_AGENT_DEVICE_I2S_SLAVE_MIN,
    HAL_AUDIO_AGENT_DEVICE_I2S1_SLAVE               = 19,
    HAL_AUDIO_AGENT_DEVICE_I2S2_SLAVE               = 20,
    HAL_AUDIO_AGENT_DEVICE_I2S_SLAVE_MAX            = HAL_AUDIO_AGENT_DEVICE_I2S2_SLAVE,

    HAL_AUDIO_AGENT_DEVICE_I2S_MASTER_MIN           = 21,
    HAL_AUDIO_AGENT_DEVICE_I2S0_MASTER              = HAL_AUDIO_AGENT_DEVICE_I2S_MASTER_MIN,
    HAL_AUDIO_AGENT_DEVICE_I2S1_MASTER              = 22,
    HAL_AUDIO_AGENT_DEVICE_I2S2_MASTER              = 23,
    HAL_AUDIO_AGENT_DEVICE_I2S_MASTER_MAX           = HAL_AUDIO_AGENT_DEVICE_I2S2_MASTER,

    HAL_AUDIO_AGENT_DEVICE_ADDA_MIN                 = 24,
    HAL_AUDIO_AGENT_DEVICE_ADDA_DL1                 = HAL_AUDIO_AGENT_DEVICE_ADDA_MIN,
    HAL_AUDIO_AGENT_DEVICE_ADDA_UL1                 = 25,
    HAL_AUDIO_AGENT_DEVICE_ADDA_UL2                 = 26,
    HAL_AUDIO_AGENT_DEVICE_ADDA_UL3                 = 27,
    HAL_AUDIO_AGENT_DEVICE_ADDA_UL4                 = 28,
    HAL_AUDIO_AGENT_DEVICE_ADDA_MAX                 = HAL_AUDIO_AGENT_DEVICE_ADDA_UL4,

    HAL_AUDIO_AGENT_DEVICE_SIDETONE                 = 29,
    HAL_AUDIO_AGENT_DEVICE_ANC                      = 30,
    HAL_AUDIO_AGENT_DEVICE_VAD                      = 31,
    HAL_AUDIO_AGENT_DEVICE_VOW                      = 32,
    HAL_AUDIO_AGENT_DEVICE_MAX                      = HAL_AUDIO_AGENT_DEVICE_VOW,

    HAL_AUDIO_AGENT_DEVICE_INPUT                    = 33,
    HAL_AUDIO_AGENT_DEVICE_OUTPUT                   = 34,

    HAL_AUDIO_AGENT_NUMBERS                         = 35,
    HAL_AUDIO_AGENT_MEMORY_NUMBERS                  = HAL_AUDIO_AGENT_MEMORY_SRC_MAX-HAL_AUDIO_AGENT_MEMORY_DL_MIN+1,
    HAL_AUDIO_AGENT_DEVICE_NUMBERS                  = HAL_AUDIO_AGENT_DEVICE_MAX-HAL_AUDIO_AGENT_DEVICE_MIN+1,
    HAL_AUDIO_AGENT_ERROR                           = 0xFF,
} hal_audio_agent_t;

typedef enum  {
    AFE_GENERAL_SAMPLERATE_8K   = 0,
    AFE_GENERAL_SAMPLERATE_11K  = 1,
    AFE_GENERAL_SAMPLERATE_12K  = 2,
    AFE_GENERAL_SAMPLERATE_16K  = 4,
    AFE_GENERAL_SAMPLERATE_22K  = 5,
    AFE_GENERAL_SAMPLERATE_24K  = 6,
    AFE_GENERAL_SAMPLERATE_32K  = 8,
    AFE_GENERAL_SAMPLERATE_44K  = 9,
    AFE_GENERAL_SAMPLERATE_48K  = 10,
    AFE_GENERAL_SAMPLERATE_88K  = 11,
    AFE_GENERAL_SAMPLERATE_96K  = 12,
    AFE_GENERAL_SAMPLERATE_176K = 13,
    AFE_GENERAL_SAMPLERATE_192K = 14,
} afe_samplerate_general_t;

typedef enum {
    AFE_ADDA_DL_SAMPLERATE_8K    = 0,
    AFE_ADDA_DL_SAMPLERATE_11K   = 1,
    AFE_ADDA_DL_SAMPLERATE_12K   = 2,
    AFE_ADDA_DL_SAMPLERATE_16K   = 3,
    AFE_ADDA_DL_SAMPLERATE_22K   = 4,
    AFE_ADDA_DL_SAMPLERATE_24K   = 5,
    AFE_ADDA_DL_SAMPLERATE_32K   = 6,
    AFE_ADDA_DL_SAMPLERATE_44K   = 7,
    AFE_ADDA_DL_SAMPLERATE_48K   = 8,
    AFE_ADDA_DL_SAMPLERATE_96K   = 9,
    AFE_ADDA_DL_SAMPLERATE_192K  = 10
}afe_samplerate_adda_dl_t;

typedef enum {
    AFE_ADDA_UL_SAMPLERATE_8K    = 0,
    AFE_ADDA_UL_SAMPLERATE_16K   = 1,
    AFE_ADDA_UL_SAMPLERATE_32K   = 2,
    AFE_ADDA_UL_SAMPLERATE_48K   = 3,
    AFE_ADDA_UL_SAMPLERATE_96K   = 4,
    AFE_ADDA_UL_SAMPLERATE_192K  = 5,
} afe_samplerate_adda_ul_t;

typedef enum {
    AFE_AUDIOSYS_IRQ0    = 0,
    AFE_AUDIOSYS_IRQ1    = 1,
    AFE_AUDIOSYS_IRQ2    = 2,
    AFE_AUDIOSYS_IRQ3    = 3,
    AFE_AUDIOSYS_IRQ4    = 4,
    AFE_AUDIOSYS_IRQ5    = 5,
    AFE_AUDIOSYS_IRQ6    = 6,
    AFE_AUDIOSYS_IRQ7    = 7,
    AFE_AUDIOSYS_IRQ9    = 9,
    AFE_AUDIOSYS_IRQ11   = 11,
    AFE_AUDIOSYS_IRQ12   = 12,
    AFE_AUDIOSYS_IRQ14   = 14,
    AFE_AUDIOSYS_IRQ_NUM,
} hal_audio_irq_audiosys_t;

#ifdef MTK_TDM_ENABLE
typedef enum {
    AFE_I2S_SLAVE_IRQ_UL    = 0,
    AFE_I2S_SLAVE_IRQ_DL    = 1,
    AFE_I2S_SLAVE_IRQ_NUM,
} hal_audio_irq_i2s_slave_t;
#endif

typedef enum  {
    AFE_SRC_1 = 0,
    AFE_SRC_2 = 1,
    AFE_SRC_NUMBER = 2
} afe_asrc_id_t;

typedef enum {
    AFE_SRC_NO_TRACKING,
    AFE_SRC_TRACKING_MODE_RX,
    AFE_SRC_TRACKING_MODE_TX,
    AFE_SRC_CONTINUOUS,
}afe_src_mode_t;


typedef enum {
    AFE_HW_DIGITAL_GAIN1 = 0,
    AFE_HW_DIGITAL_GAIN2,
    AFE_HW_DIGITAL_GAIN3,
    AFE_HW_DIGITAL_GAIN_NUM,
} afe_hardware_digital_gain_t;

typedef enum {
    AFE_HW_ANALOG_GAIN_OUTPUT = 0,
    AFE_HW_ANALOG_GAIN_INPUT1,
    AFE_HW_ANALOG_GAIN_INPUT2,
    AFE_HW_ANALOG_GAIN_INPUT3,
    AFE_HW_ANALOG_GAIN_NUM,
} afe_hardware_analog_gain_t;

typedef enum  {
    AFE_I2S0 = 0,
    AFE_I2S1,
    AFE_I2S2,
    AFE_I2S_NUMBER,
} afe_i2s_id_t;

typedef enum  {
    AFE_SINE_GENERATOR_AMPLITUDE_DIVIDE_128 = 0,
    AFE_SINE_GENERATOR_AMPLITUDE_DIVIDE_64,
    AFE_SINE_GENERATOR_AMPLITUDE_DIVIDE_32,
    AFE_SINE_GENERATOR_AMPLITUDE_DIVIDE_16,
    AFE_SINE_GENERATOR_AMPLITUDE_DIVIDE_8,
    AFE_SINE_GENERATOR_AMPLITUDE_DIVIDE_4,
    AFE_SINE_GENERATOR_AMPLITUDE_DIVIDE_2,
    AFE_SINE_GENERATOR_AMPLITUDE_DIVIDE_1,
} afe_sine_generator_amplitude_t;

typedef enum  {
    AFE_TICK_ALIGN_GENERAL_SLAVE_I2S0_IN = 0,
    AFE_TICK_ALIGN_GENERAL_SLAVE_I2S1_IN,
    AFE_TICK_ALIGN_GENERAL_SLAVE_I2S2_IN,
    AFE_TICK_ALIGN_GENERAL_SLAVE_I2S0_OUT,
    AFE_TICK_ALIGN_GENERAL_SLAVE_I2S1_OUT,
    AFE_TICK_ALIGN_GENERAL_SLAVE_I2S2_OUT,
} afe_tick_align_general_t;

typedef enum  {
    AFE_TICK_ALIGN_IRQ_SLAVE_I2S0_IN = 0,
    AFE_TICK_ALIGN_IRQ_SLAVE_I2S1_IN,
    AFE_TICK_ALIGN_IRQ_SLAVE_I2S2_IN,
    AFE_TICK_ALIGN_IRQ_SLAVE_I2S0_OUT,
    AFE_TICK_ALIGN_IRQ_SLAVE_I2S1_OUT,
    AFE_TICK_ALIGN_IRQ_SLAVE_I2S2_OUT,
    AFE_TICK_ALIGN_IRQ_MEMORY_AGENT,
} afe_tick_align_irq_t;

typedef enum  {
    AFE_TICK_ALIGN_GAIN_SLAVE_I2S0_IN = 0,
    AFE_TICK_ALIGN_GAIN_SLAVE_I2S1_IN,
    AFE_TICK_ALIGN_GAIN_SLAVE_I2S2_IN,
    AFE_TICK_ALIGN_GAIN_SLAVE_I2S0_OUT,
    AFE_TICK_ALIGN_GAIN_SLAVE_I2S1_OUT,
    AFE_TICK_ALIGN_GAIN_SLAVE_I2S2_OUT,
} afe_tick_align_gain_t;

typedef enum  {
    AFE_TICK_ALIGN_UL_SLAVE_I2S0_IN     = 0,
    AFE_TICK_ALIGN_UL_SLAVE_I2S1_IN     = 1,
    AFE_TICK_ALIGN_UL_SLAVE_I2S2_IN     = 2,

    AFE_TICK_ALIGN_UL_DOWNSAMPLER_CH0   = 4,
    AFE_TICK_ALIGN_UL_DOWNSAMPLER_CH1   = 5,
    AFE_TICK_ALIGN_UL_DOWNSAMPLER_CH2   = 6,
    AFE_TICK_ALIGN_UL_DOWNSAMPLER_CH3   = 7,
} afe_tick_align_ul_t;

typedef enum  {
    AFE_TICK_ALIGN_DL_SLAVE_I2S0_OUT    = 0,
    AFE_TICK_ALIGN_DL_SLAVE_I2S1_OUT    = 1,
    AFE_TICK_ALIGN_DL_SLAVE_I2S2_OUT    = 2,
} afe_tick_align_dl_t;

typedef enum  {
    AFE_TICK_ALIGN_DOWNSAMPLER_SLAVE_I2S0_IN = 0,
    AFE_TICK_ALIGN_DOWNSAMPLER_SLAVE_I2S1_IN,
    AFE_TICK_ALIGN_DOWNSAMPLER_SLAVE_I2S2_IN,
    AFE_TICK_ALIGN_DOWNSAMPLER_SLAVE_I2S0_OUT,
    AFE_TICK_ALIGN_DOWNSAMPLER_SLAVE_I2S1_OUT,
    AFE_TICK_ALIGN_DOWNSAMPLER_SLAVE_I2S2_OUT,
    AFE_TICK_ALIGN_DOWNSAMPLER_OUTPUT_CH0,
    AFE_TICK_ALIGN_DOWNSAMPLER_OUTPUT_CH1,
} afe_tick_align_downsampler_t;

typedef enum {
    AFE_UPDOWN_SAMPLER_UP_CH01 = 0,
    AFE_UPDOWN_SAMPLER_DOWN_CH01,
    AFE_UPDOWN_SAMPLER_DOWN_CH23,
} afe_updown_sampler_id_t;


typedef enum {
    AFE_I2S_APLL_NOUSE = 0,
    AFE_I2S_APLL1 = 1,      /* 45.1584MHz for 44.1KHz base */
    AFE_I2S_APLL2 = 2,      /* 49.152MHz  for 48kHz base */
} afe_i2s_apll_t;

typedef enum {
    AFE_ANALOG_COMMON   = 1<<0,
    AFE_ANALOG_L_CH     = 1<<1,
    AFE_ANALOG_R_CH     = 1<<2,
} afe_analog_control_t;

typedef enum {
    AFE_ANALOG_DAC      = 0,
    AFE_ANALOG_ADC0     = 1,
    AFE_ANALOG_ADC1     = 2,    //Only for AB1568
    AFE_ANALOG_ADC2     = 3,    //Only for AB1568
    AFE_ANALOG_NUMBER,
} afe_analog_select_t;

/* hal audio handler entry structure */
typedef void (*hal_audio_handler_entry)(void);

typedef struct  {
    uint32_t addr;        /* physical */
    uint32_t size;
    uint32_t rate;
    uint32_t offset;
    hal_audio_pcm_format_t format;
} afe_src_buffer_t;

typedef struct {
    afe_src_buffer_t input_buffer;
    afe_src_buffer_t output_buffer;
    afe_asrc_id_t id;
    afe_src_mode_t mode;
    hal_audio_src_tracking_clock_t tracking_clock;

    uint16_t sample_count_threshold; //Units:sample

    bool is_mono;
    bool hw_update_obuf_rdpnt;
    bool ul_mode;
#ifdef ENABLE_HWSRC_CLKSKEW
    hal_audio_src_clk_skew_mode_t clkskew_mode;
#endif
} afe_src_configuration_t, *afe_src_configuration_p;

typedef struct {
    int32_t base_value;
    int32_t compensation_value;
    int32_t step_value;
    int32_t step;
} afe_src_clock_compensation_t, *afe_src_clock_compensation_p;

typedef struct {
    uint32_t input_rate;
    uint32_t output_rate;
    hal_audio_path_interconnection_tick_source_t tick_align;
    bool is_echo_configure_input;
} afe_updown_configuration_t, *afe_updown_configuration_p;


typedef struct {
    HAL_AUDIO_TIMER_HANDLE          timer_handler;
    uint32_t                        delay_output_off_time_ms;
    uint32_t                        delay_gpio_on_time_ms;
    hal_audio_agent_t               agent;
    hal_audio_device_parameter_t    device_parameter;

} afe_device_delay_parameter_t, *afe_device_delay_parameter_p;


/* Audio SRAM structure */
typedef struct
{
    uint32_t                sram_addr;
    hal_audio_agent_t       user;
} hal_audio_memory_sram_block_t, *hal_audio_memory_sram_block_p;

typedef union {
    struct
    {
        int16_t    channel_l;
        int16_t    channel_r;
    } channel_counter;
    int16_t    channel[2];
    int32_t    counter;
} afe_analog_channel_control_t, *afe_analog_channel_control_p;

/* hal audio sidetone control structure */
typedef struct
{
    HAL_AUDIO_TIMER_HANDLE                  timer_handle;
    hal_audio_closure_entry                 sidetone_stop_done_entry;
    hal_audio_path_interconnection_input_t  input_interconn_select;
    int32_t                                 gain_step;
    int32_t                                 target_positive_gain;
    int32_t                                 target_negative_gain;
    int32_t                                 current_positive_gain;
    int32_t                                 current_negative_gain;
    bool                                    with_ramp_control;
    bool                                    ramp_start_delay;
    bool                                    ramp_done;
    bool                                    ramp_down_done;
    bool                                    tick_align_enable;
} hal_audio_sidetone_control_t;

/* hal audio vad control structure */
typedef struct
{
    HAL_AUDIO_TIMER_HANDLE          timer_handle;
} hal_audio_vad_control_t;
typedef struct
{
    hal_audio_memory_sram_block_t   block[HAL_AUDIO_MEMORY_SRAM_BLOCK_NUMBER];
    OS_STRU_SEMAPHORE               semaphore;
    OS_STRU_SEMAPHORE_PTR           semaphore_ptr;
    uint16_t                        remain_block;
    bool                            is_initialized;
} hal_audio_memory_sram_control_t, *hal_audio_memory_sram_control_p;

/* hal audio amp control structure */
typedef struct
{
    uint32_t                        output_gpio;
    hal_audio_handler_entry         notice_off_handler;
    afe_device_delay_parameter_t    delay_handle;
    bool                            output_gpio_status;
    bool                            is_hold_output_gpio;
} hal_audio_amp_control_t;

/* hal audio vow control structure */
typedef struct
{
    HAL_AUDIO_TIMER_HANDLE          timer_handle;
    hal_audio_vow_mode_t            vow_mode;
    uint32_t                        stable_noise;
    uint32_t                        noise_ignore_bit;
    uint32_t                        u4AFE_MEMIF_BUF_BASE;
    uint32_t                        u4AFE_MEMIF_BUF_END;
    uint32_t                        u4AFE_MEMIF_BUF_WP;
    uint32_t                        u4AFE_MEMIF_BUF_RP;
    bool                            is_stable;
    bool                            first_snr_irq;
} hal_audio_vow_control_t;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Variables ///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function Prototypes ////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************************
*                                      Audio Control                                       *
********************************************************************************************/
void hal_audio_control_initialize(void);
uint32_t hal_audio_control_get_value(hal_audio_get_value_parameter_t *handle, hal_audio_get_value_command_t command);
hal_audio_status_t hal_audio_control_set_value(hal_audio_set_value_parameter_t *handle, hal_audio_set_value_command_t command);


/*******************************************************************************************
*                                       Device agent                                       *
********************************************************************************************/
int hal_audio_device_semaphore_take(HAL_AUDIO_SEMAPHO_HANDLE xSemaphore,uint32_t xBlockTime);
int hal_audio_device_semaphore_give(HAL_AUDIO_SEMAPHO_HANDLE xSemaphore);
bool hal_audio_device_set_agent(hal_audio_device_parameter_t *handle, hal_audio_control_t device, hal_audio_control_status_t control);
void hal_audio_enter_criticl_section(void);
void hal_audio_exit_criticl_section(void);
/*******************************************************************************************
*                                       Memory agent                                       *
********************************************************************************************/
bool hal_audio_memory_set_agent(hal_audio_memory_parameter_t *handle, hal_audio_control_t memory_interface, hal_audio_control_status_t control);
bool hal_audio_memory_sw_trigger(hal_audio_memory_selection_t memory_select, bool enable);
hal_audio_memory_selection_t hal_audio_memory_convert_interconn_select_to_memory_selecct(hal_audio_interconn_selection_t interconn_select);
bool hal_audio_memory_get_info(hal_audio_memory_information_parameter_t *memory_info);


void hal_memory_initialize_sram(void);
uint32_t hal_memory_allocate_sram(hal_audio_agent_t user_type, uint32_t size);
uint32_t hal_memory_free_sram(hal_audio_agent_t user_type);


/*******************************************************************************************
*                                         HW gain                                          *
********************************************************************************************/
bool hal_audio_hardware_gain_set_agent (afe_hardware_digital_gain_t gain_select, uint32_t samplerate, hal_audio_control_status_t control);
int32_t hal_audio_hardware_gain_get_agent_count (afe_hardware_digital_gain_t gain_select);
afe_hardware_digital_gain_t hal_audio_hardware_gain_get_selcet (hal_audio_memory_selection_t memory_selection);


/*******************************************************************************************
*                                     Up/Down Sampler                                      *
********************************************************************************************/
bool hal_audio_updown_set_agent (afe_updown_configuration_t *configure, afe_updown_sampler_id_t updown_id, hal_audio_control_status_t control);
int32_t hal_audio_updown_get_agent_count (afe_updown_sampler_id_t updown_id);
bool hal_audio_updown_set_configuration(afe_updown_sampler_id_t updown_id, uint32_t input_rate, uint32_t output_rate);
bool hal_audio_updown_set_start(afe_updown_sampler_id_t updown_id, hal_audio_control_status_t control);


/*******************************************************************************************
*                                           IRQ                                            *
********************************************************************************************/
void hal_audio_irq_register(hal_audio_irq_parameter_t *irq_parameter);
void hal_audio_irq_initialize(void);


/*******************************************************************************************
*                                     UL/DL device                                         *
********************************************************************************************/
bool hal_audio_adda_set_dl(hal_audio_agent_t device_agent, uint32_t samplerate, hal_audio_dl_sdm_setting_t sdm_setting, hal_audio_control_status_t control);
hal_audio_control_t hal_audio_device_analog_get_output(afe_analog_select_t analog_select);


#endif //#ifdef HAL_AUDIO_MODULE_ENABLED
#endif /* __HAL_AUDIO_CONTROL_H__ */
