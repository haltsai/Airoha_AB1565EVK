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

#ifndef __HAL_AUDIO_CLOCK_H__
#define __HAL_AUDIO_CLOCK_H__

#include "hal_audio.h"
#ifdef HAL_AUDIO_MODULE_ENABLED

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Type Definitions ///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Variables ///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function Prototypes ////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void hal_audio_clock_initialize(void);


void hal_audio_clock_enable_afe(bool enable);
void hal_audio_clock_enable_i2s_slave_hclk(bool enable);
void hal_audio_clock_enable_22m(bool enable);
void hal_audio_clock_enable_24m(bool enable);
void hal_audio_clock_enable_apll2(bool enable);
void hal_audio_clock_enable_apll(bool enable);
void hal_audio_clock_enable_adc3(bool enable);
void hal_audio_clock_enable_adc2(bool enable);
void hal_audio_clock_enable_adc(bool enable);
void hal_audio_clock_enable_dac(bool enable);
void hal_audio_clock_enable_sine(bool enable);
void hal_audio_clock_enable_classg(bool enable);

void hal_audio_clock_enable_i2s0(bool enable);
void hal_audio_clock_enable_i2s1(bool enable);
void hal_audio_clock_enable_i2s2(bool enable);
void hal_audio_clock_enable_i2s3(bool enable);
void hal_audio_clock_enable_i2s_dma(bool enable);
void hal_audio_clock_enable_i2s_dma_irq(bool enable);
void hal_audio_clock_enable_adc_hires(bool enable);
void hal_audio_clock_enable_sine_hire(bool enable);
void hal_audio_clock_enable_adda2(bool enable);
void hal_audio_clock_enable_adda6(bool enable);
void hal_audio_clock_enable_adda_anc(bool enable);
void hal_audio_clock_enable_dac_hires(bool enable);
void hal_audio_clock_enable_src(bool enable);
void hal_audio_clock_enable_src1(bool enable);
void hal_audio_clock_enable_src2(bool enable);




void hal_audio_afe_set_enable(bool enable);
uint32_t hal_audio_afe_get_counter(void);
void hal_audio_afe_enable_clksq(bool enable);
void hal_audio_clock_set_dac(bool enable);






#endif //#ifdef HAL_AUDIO_MODULE_ENABLED
#endif /* __HAL_AUDIO_CLOCK_H__ */
