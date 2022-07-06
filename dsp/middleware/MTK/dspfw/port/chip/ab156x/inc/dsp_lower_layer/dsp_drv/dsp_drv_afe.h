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

#ifndef __DSP_DRV_AFE_H__
#define __DSP_DRV_AFE_H__

#include "types.h"
#include "dsp_drv_afe_hal.h"
#include "dsp_control.h"
#include "hal_audio.h"
#include "hal_audio_afe_control.h"
#include "hal_audio_afe_define.h"
#include "common.h"
#include "timers.h"
#include "semphr.h"

#ifdef ENABLE_SIDETONE_RAMP_TIMER
#define FW_SIDETONE_MUTE_GAIN       (-6000)
#define FW_SIDETONE_RAMP_STEP       (60)
#define FW_SIDETONE_RAMP_TIMER      (10)
#endif

/******************************************************************************
 * Type Definitions
 ******************************************************************************/
typedef enum AU_AFE_IN_CH_e
{
    AU_AFE_IN_CH_NONE = 0,
    AU_AFE_IN_CH_1    = 1,
    AU_AFE_IN_CH_2    = 2,
} AU_AFE_IN_CH_t;

typedef enum AU_AFE_OUT_CH_e
{
    AU_AFE_OUT_CH_NONE = 0,
    AU_AFE_OUT_CH_1    = 1,
    AU_AFE_OUT_CH_2    = 2,
} AU_AFE_OUT_CH_t;


typedef enum AU_AFE_OUT_MODE_e
{
    AU_AFE_OUT_LRVCM_MODE         = 0,
    AU_AFE_OUT_DIFFERENTIAL_MODE  = 1,
    AU_AFE_OUT_SINGLE_END_MODE    = 2,
} AU_AFE_OUT_MODE_t;

typedef enum AU_AFE_OP_MODE_e
{
    AU_AFE_OP_A2DP_MODE             = 0,  /* Only Speaker Out */
    AU_AFE_OP_ESCO_VOICE_MODE       = 1,  /* Mic In, Speaker Out */
    AU_AFE_OP_LINE_DSP_MODE         = 2,  /* Line In, Speaker Out */
    AU_AFE_OP_LINE_BYPASS_MODE      = 3,  /* Analog Bypass */
    AU_AFE_OP_CAPTURE_MODE          = 4,  /* Only Mic In */
    AU_AFE_OP_ANC_FF_MODE           = 5,  /* ANC Feedback */
    AU_AFE_OP_ANC_FB_MODE           = 6,  /* ANC Feed-Forward */
    AU_AFE_OP_VP_MODE               = 7,  /* Voice prompt */
    AU_AFE_OP_PLAYBACK_MODE         = 8,  /* Only Speaker Out */
    AU_AFE_OP_MODE_MAX,
} AU_AFE_OP_MODE_t;

typedef struct {
    uint32_t                audio_init;
    uint32_t                IsAdcClosedBefore;
} AfeDriverCtrl_t;

typedef struct {
    uint32_t rate;          /* rate in Hz       */
    uint32_t src_rate;
    float period;        /* delay in ms      */
    afe_pcm_format_t format;

    hal_audio_device_t               audio_device;
#ifdef ENABLE_2A2D_TEST
    hal_audio_device_t               audio_device1;
    hal_audio_device_t               audio_device2;
    hal_audio_device_t               audio_device3;
    hal_audio_device_t               audio_device4;
    hal_audio_device_t               audio_device5;
    hal_audio_device_t               audio_device6;
    hal_audio_device_t               audio_device7;
#endif
    hal_audio_channel_selection_t    stream_channel;
    hal_audio_memory_t                      memory;
    hal_audio_interface_t                   audio_interface;
#ifdef ENABLE_2A2D_TEST
    hal_audio_interface_t                   audio_interface1;
    hal_audio_interface_t                   audio_interface2;
    hal_audio_interface_t                   audio_interface3;
    hal_audio_interface_t                   audio_interface4;
    hal_audio_interface_t                   audio_interface5;
    hal_audio_interface_t                   audio_interface6;
    hal_audio_interface_t                   audio_interface7;
#endif
    afe_misc_parms_t                        misc_parms;
    bool                                    hw_gain;
    bool                                    echo_reference;
#ifdef HAL_AUDIO_ENABLE_PATH_MEM_DEVICE
    hal_audio_analog_mdoe_t                 adc_mode;
    hal_audio_i2s_format_t                  i2s_format;
#ifdef MTK_TDM_ENABLE
    hal_audio_i2s_tdm_channel_setting_t     tdm_channel;
#else
    bool                                    i2S_Slave_TDM;
#endif
    hal_audio_i2s_word_length_t             i2s_word_length;
    bool                                    is_low_jitter;
    hal_audio_performance_mode_t            performance;
    uint8_t                                 ul_adc_mode[8];
    hal_audio_bias_voltage_t                bias_voltage[5];/**hal_audio_bias_voltage_t*/
    hal_audio_bias_selection_t              bias_select;
    uint8_t                                 with_external_bias;
    uint8_t                                 with_bias_lowpower;
    bool                                    bias1_2_with_LDO0;
    hal_audio_dmic_selection_t              dmic_selection[8];/**< hal_audio_dmic_selection_t */
    hal_audio_ul_iir_t                      iir_filter[3];/**hal_audio_ul_iir_t*/
#ifdef AIR_HFP_DNN_PATH_ENABLE
    bool                                    enable_ul_dnn;
#endif
    uint8_t                                 dmic_clock_rate[3];/**afe_dmic_clock_rate_t*/
#endif
    bool                                    with_upwdown_sampler; /**for up or down sampler*/
    uint32_t                                audio_path_input_rate; /**for up or down sampler*/
    uint32_t                                audio_path_output_rate; /**for up or down sampler*/
} AfeIRQAgentCtrl_t;

typedef struct AU_AFE_CTRL_s
{
    U32                     ForceSet;
    U32                     UseAncMic;
    AU_AFE_IN_CH_t          InputChannel;
    AU_AFE_OUT_CH_t         OutputChannel;
    AU_AFE_OUT_MODE_t       OutputMode;
    AU_AFE_SPK_VOLTAGE_t    VccSpkVoltage;
    AU_AFE_OP_MODE_t        OperationMode;
    hal_audio_device_t      DeviceInType;
    hal_audio_device_t      DeviceOutType;
    AfeDriverCtrl_t         AfeControl;
    AfeIRQAgentCtrl_t       AfeDLSetting;
    AfeIRQAgentCtrl_t       AfeULSetting;
} AU_AFE_CTRL_t;

#ifdef ENABLE_AMP_TIMER
typedef enum {
    FW_AMP_TIMER_STOP       = 0,
    FW_AMP_TIMER_START,
    FW_AMP_TIMER_END,
} fw_amp_timer_status_t;


typedef struct {
    fw_amp_timer_status_t   status;
    TimerHandle_t           timer;
    SemaphoreHandle_t       semaphore;
} fw_amp_config_t, *fw_amp_config_p;
#endif

#ifdef ENABLE_SIDETONE_RAMP_TIMER
typedef struct {
    int32_t                 current_gain;
    int32_t                 target_gain;
    TimerHandle_t           timer;
} fw_sidetone_ramp_config_t, *fw_sidetone_ramp_config_p;
#endif

/******************************************************************************
 * External Function Prototypes
 ******************************************************************************/
EXTERN VOID DSP_DRV_AFE_Init (VOID);
EXTERN VOID DSP_DRV_AFE_ChangeOperationMode (AU_AFE_OP_MODE_t Mode);
EXTERN VOID DSP_DRV_AFE_ChangeOutputChannel (AU_AFE_OUT_CH_t Channel);
EXTERN VOID DSP_DRV_AFE_SetOutputGain (AU_AFE_OUT_GAIN_COMPONENT_t Component, S32 Gain);
EXTERN VOID DSP_DRV_AFE_SetInputGain (AU_AFE_IN_GAIN_COMPONENT_t Component, U8 Gain);

void DSP_DRV_AFE_SetInputDevice(hal_ccni_message_t msg, hal_ccni_message_t *ack);
void DSP_DRV_AFE_SetOutputDevice(hal_ccni_message_t msg, hal_ccni_message_t *ack);
void DSP_DRV_AFE_SetOutputChannel(hal_ccni_message_t msg, hal_ccni_message_t *ack);
void DSP_DRV_AFE_SetInputChannel(hal_ccni_message_t msg, hal_ccni_message_t *ack);

#ifdef ENABLE_AMP_TIMER
void fw_amp_init_semaphore (void);
void fw_amp_init_timer (void);
void fw_amp_set_status(fw_amp_timer_status_t status);
fw_amp_timer_status_t fw_amp_get_status(void);
bool fw_amp_timer_start(void);
bool fw_amp_timer_stop(uint32_t samplerate);
bool fw_amp_force_close(void);
#endif

#ifdef ENABLE_SIDETONE_RAMP_TIMER
void fw_sidetone_ramp_init (void);
void fw_sidetone_set_ramp_timer (int32_t target_gain);
#endif

/*Parameters transform
audio_hardware afe_get_audio_hardware_by_au_afe_open_param (au_afe_open_param_t afe_open);
audio_instance afe_get_audio_instance_by_au_afe_open_param (au_afe_open_param_t afe_open);
audio_channel afe_get_audio_channel_by_au_afe_open_param (au_afe_open_param_t afe_open);

*/

void afe_set_hold_amp_gpio (uint32_t is_hold);


#endif /* __DSP_DRV_AFE_H__ */

