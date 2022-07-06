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

#ifndef __LINEIN_PLAYBACK_H__
#define __LINEIN_PLAYBACK_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <hal_platform.h>
#include "hal_audio.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    LINEIN_PLAYBACK_EVENT_OPEN,
    LINEIN_PLAYBACK_EVENT_START,
    LINEIN_PLAYBACK_EVENT_STOP,
    LINEIN_PLAYBACK_EVENT_CLOSE,
    LINEIN_PLAYBACK_EVENT_SET_VOLUME,
} linein_playback_event_t;

typedef enum {
    LINEIN_EXECUTION_FAIL    = -1,
    LINEIN_EXECUTION_SUCCESS =  0,
} linein_result_t;

/** @brief linein playback state. */
typedef enum {
    LINEIN_STATE_ERROR = -1,     /**< An error occurred. */
    LINEIN_STATE_IDLE,           /**< The linein playback is inactive. */
    LINEIN_STATE_READY,          /**< The linein playback is ready to play the media. */
    LINEIN_STATE_PLAY,           /**< The linein playback is in playing state. */
    LINEIN_STATE_STOP,           /**< The linein playback has stopped. */
} linein_playback_state_t;

typedef struct {
    uint32_t    in_digital_gain;    /**< Digital gain index of the audio stream in.*/
    uint32_t    in_analog_gain;     /**< Analog gain index of the audio stream in.*/
    uint32_t    out_digital_gain;   /**< Digital gain index of the audio stream out.*/
    uint32_t    out_analog_gain;    /**< Analog gain index of the audio stream out.*/
} linein_playback_gain_t;

#ifdef AIR_LE_AUDIO_ENABLE
linein_result_t linein_playback_le_audio_open(hal_audio_sampling_rate_t linein_sample_rate, hal_audio_sampling_rate_t dacout_sample_rate, uint16_t frame_sample_count);
#endif
linein_result_t linein_playback_open(hal_audio_sampling_rate_t linein_sample_rate, hal_audio_device_t in_audio_device, hal_audio_device_t out_audio_device);
linein_result_t linein_playback_start();
linein_result_t linein_playback_stop();
linein_result_t linein_playback_close();
linein_result_t linein_playback_set_volume(linein_playback_gain_t gain);

linein_result_t audio_linein_playback_open(hal_audio_sampling_rate_t linein_sample_rate, hal_audio_device_t in_audio_device, hal_audio_device_t out_audio_device);
linein_result_t audio_linein_playback_start();
linein_result_t audio_linein_playback_stop();
linein_result_t audio_linein_playback_close();
linein_result_t audio_linein_playback_set_volume(linein_playback_gain_t gain);

linein_result_t pure_linein_playback_open(hal_audio_sampling_rate_t linein_sample_rate, hal_audio_device_t in_audio_device, hal_audio_device_t out_audio_device);
linein_result_t pure_linein_playback_close();
linein_result_t audio_pure_linein_playback_open(hal_audio_sampling_rate_t linein_sample_rate, hal_audio_device_t in_audio_device, hal_audio_device_t out_audio_device);
linein_result_t audio_pure_linein_playback_close();
#ifdef LINE_IN_PURE_FOR_AMIC_CLASS_G_HQA
linein_result_t audio_pure_linein_playback_open_HQA(hal_audio_sampling_rate_t linein_sample_rate, hal_audio_device_t in_audio_device,hal_audio_interface_t device_in_interface_HQA,hal_audio_analog_mdoe_t adc_mode_HQA,hal_audio_performance_mode_t mic_performance_HQA, hal_audio_device_t out_audio_device,hal_audio_performance_mode_t dac_performance_HQA);
#endif

#ifdef LINE_IN_PURE_FOR_AMIC_CLASS_G_HQA
linein_result_t pure_linein_playback_open_HQA(hal_audio_sampling_rate_t linein_sample_rate, hal_audio_device_t in_audio_device,hal_audio_interface_t device_in_interface_HQA,hal_audio_analog_mdoe_t adc_mode_HQA,hal_audio_performance_mode_t mic_performance_HQA, hal_audio_device_t out_audio_device,hal_audio_performance_mode_t dac_performance_HQA);
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__LINEIN_PLAYBACK_H__*/
