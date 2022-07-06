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

#ifndef __HAL_AUDIO_AFE_CLK_H__
#define __HAL_AUDIO_AFE_CLK_H__

#include "hal_audio.h"
#include "hal_audio_control.h"//modify for ab1568
#ifdef HAL_AUDIO_MODULE_ENABLED

#include <stdint.h>

typedef enum {
    AFE_APLL_NOUSE = 0,
    AFE_APLL1 = 1,  /* 44.1K base */
    AFE_APLL2 = 2,  /* 48base */
} afe_apll_source_t;

void hal_audio_afe_clock_on(void);
void hal_audio_afe_clock_off(void);
#if 0//modify for ab1568
void hal_audio_afe_i2s_clock_on(afe_i2s_num_t i2s_num);
void hal_audio_afe_i2s_clock_off(afe_i2s_num_t i2s_num);
#endif
void afe_clock_variable_init(void);

void afe_dac_clock_on(void);
void afe_dac_clock_off(void);
int16_t afe_get_dac_clock_status(void);
void afe_adc_clock_on(void);
void afe_adc_clock_off(void);
void afe_adc2_clock_on(void);
void afe_adc2_clock_off(void);
void afe_adc6_clock_on(void);
void afe_adc6_clock_off(void);
void afe_classg_clock_on(void);
void afe_classg_clock_off(void);
void afe_dac_hires_clock(bool pdn);
void afe_adc_hires_clock(bool pdn);

/*APLL realted APIs*/
void afe_apll1tuner_clock_on(void);
void afe_apll1tuner_clock_off(void);
void afe_apll2tuner_clock_on(void);
void afe_apll2tuner_clock_off(void);
void afe_apll1_enable(bool enable);
void afe_apll2_enable(bool enable);

afe_apll_source_t afe_get_apll_by_samplerate(uint32_t samplerate);

void afe_enable_apll_by_samplerate(uint32_t samplerate);
void afe_disable_apll_by_samplerate(uint32_t samplerate);
void afe_enable_apll_tuner_by_samplerate(uint32_t samplerate);
void afe_disable_apll_tuner_by_samplerate(uint32_t samplerate);


void afe_asrc_clock_on(afe_mem_asrc_id_t asrc_id);
void afe_asrc_clock_off(afe_mem_asrc_id_t asrc_id);

#endif //#ifdef HAL_AUDIO_MODULE_ENABLED
#endif /* __HAL_AUDIO_AFE_CLK_H__ */
