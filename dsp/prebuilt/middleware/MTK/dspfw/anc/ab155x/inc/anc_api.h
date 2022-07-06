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

#define MTK_ANC_CHANGE_DL_SRC_RATE

/*anc_control.c*/
void dsp_anc_init(void);
void dsp_anc_reset_anc_on(void);
void dsp_anc_start_chk(uint32_t arg);
void dsp_anc_stop_chk(void);
void dsp_anc_stop(hal_ccni_message_t msg, hal_ccni_message_t *ack);
void dsp_anc_start(hal_ccni_message_t msg, hal_ccni_message_t *ack);
void dsp_set_anc_param(hal_ccni_message_t msg, hal_ccni_message_t *ack);
void dsp_gc_set_anc_volume(hal_ccni_message_t msg, hal_ccni_message_t *ack);
void afe_anc_pwrdet_interrupt_handler(uint32_t irq_index);
void dsp_anc_check_dl_src_rate(uint32_t rate);
bool dsp_anc_get_dl_src_rate(uint32_t *rate);
bool dsp_anc_set_hw_gain_id(uint8_t hw_gain_id);


/*anc_driver.c*/
void afe_anc_init(void);
bool afe_get_anc_enable (hal_audio_device_t device);
BOOL afe_get_anc_ramp_done(void);
void afe_set_anc_ramp(int16_t target_gain, int16_t delta_mod_gain, uint32_t misc_param);
void afe_set_anc_hw_gain(int16_t config_gain, int16_t runtime_gain, int16_t delta_mod_gain);
void afe_get_anc_hw_gain(int16_t *config_gain, int16_t *runtime_gain);

#endif //#ifdef MTK_ANC_ENABLE
#endif //#ifndef __ANC_API_H__

