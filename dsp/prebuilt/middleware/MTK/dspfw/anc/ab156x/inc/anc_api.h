/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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

#ifndef __ANC_API_H__
#define __ANC_API_H__

#ifdef MTK_ANC_ENABLE
#include "types.h"
#include "hal_ccni.h"
#include "hal_platform.h"

/*anc_control.c*/
void dsp_anc_init(void);
void dsp_anc_stop(hal_ccni_message_t msg, hal_ccni_message_t *ack);
void dsp_anc_start(hal_ccni_message_t msg, hal_ccni_message_t *ack);
void dsp_set_anc_param(hal_ccni_message_t msg, hal_ccni_message_t *ack);
void dsp_set_anc_volume(hal_ccni_message_t msg, hal_ccni_message_t *ack);
void afe_anc_pwrdet_interrupt_handler(uint32_t irq_index);

/*hal_audio_anc.c*/
bool hal_audio_device_set_anc(hal_audio_device_parameter_t *handle, hal_audio_control_t device, hal_audio_control_status_t control);
void hal_audio_anc_set_reg(hal_audio_set_value_parameter_t *anc_set_param);
uint32_t hal_audio_anc_get_reg(hal_audio_get_value_parameter_t *anc_get_param);
void hal_audio_anc_init(void);
void hal_audio_anc_set_change_dl_rate(uint32_t rate);
bool hal_audio_anc_get_change_dl_rate(uint32_t *rate);
uint32_t hal_audio_anc_get_using_count(hal_audio_device_parameter_t *handle);

/** @brief ANC and Pass-through type values. */
typedef enum {
    DSP_ANC_CONTROL_TYPE_HYBRID       = 0,  /**< The hybrid anc type.   */
    DSP_ANC_CONTROL_TYPE_FF           = 1,  /**< The feedforward anc type.   */
    DSP_ANC_CONTROL_TYPE_FB           = 2,  /**< The feedback anc type.   */
    DSP_ANC_CONTROL_TYPE_USER_DEFINED = 3,  /**< The user defined anc type.   */
    DSP_ANC_CONTROL_TYPE_PASSTHRU_FF  = 4,  /**< The pass-through feedforward anc type.   */
} dsp_anc_control_type_t;

typedef enum {
    DSP_ANC_CALLBACK_EVENT_RAMP,
    DSP_ANC_CALLBACK_EVENT_RESERVE_1,
} dsp_anc_callback_event_t;

typedef struct
{
    uint8_t          ramp_status;
    uint32_t         ramp_gain[4];
    uint32_t         *p_reserve[4];
} dsp_anc_callback_param_t;

/** @brief The support misc settings capability structure of the ANC path. */
typedef struct dsp_anc_control_misc_s {
    uint32_t extend_use_parameters;                    /**< Extend use.   */
} dsp_anc_control_misc_t;

/** @brief hal audio device done closure entry for delay off procedure */
typedef void (*dsp_anc_notify_entry)(dsp_anc_callback_event_t event, dsp_anc_callback_param_t *param);
void dsp_set_anc_notify_callback(dsp_anc_notify_entry anc_notify_entry);
void dsp_get_anc_notify_param(dsp_anc_callback_param_t *param);
void dsp_anc_control_get_status(uint8_t *enable, dsp_anc_control_type_t *type, dsp_anc_control_misc_t *control_misc);

void anc_port_log_info(int32_t index, const char *message, uint32_t arg_cnt, ...);
void anc_port_log_notice(int32_t index, const char *message, uint32_t arg_cnt, ...);
void anc_port_log_error(int32_t index, const char *message, uint32_t arg_cnt, ...);
void anc_port_log_debug(int32_t index, const char *message, uint32_t arg_cnt, ...);

#endif //#ifdef MTK_ANC_ENABLE
#endif //#ifndef __ANC_API_H__

