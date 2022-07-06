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
#ifndef _DSP_VAD_H_
#define _DSP_VAD_H_

#include "config.h"
#include "types.h"
//#include "os_semaphore.h"
#include "hal_audio.h"
//#include "os_list.h"
#include "source_inter.h"

#define FEA_SUPP_DSP_VAD  0
/**
 * @brief Parameter for Audio AFE VAD
 */
typedef enum afe_vad_input_select_e
{
    AFE_VAD_INPUT_MIC0_P = HAL_AUDIO_VAD_INPUT_MIC0_P,  //0
    AFE_VAD_INPUT_MIC0_N = HAL_AUDIO_VAD_INPUT_MIC0_N,  //1
    AFE_VAD_INPUT_MIC1_P = HAL_AUDIO_VAD_INPUT_MIC1_P,  //2
    AFE_VAD_INPUT_MIC1_N = HAL_AUDIO_VAD_INPUT_MIC1_N,  //3
} afe_vad_input_select_t;


typedef enum afe_vad_amp_gain_e
{
    AFE_VAD_GAIN_POS_0DB = 0,
    AFE_VAD_GAIN_POS_3DB,
    AFE_VAD_GAIN_POS_6DB,
    AFE_VAD_GAIN_POS_9DB,
    AFE_VAD_GAIN_POS_12DB,
    AFE_VAD_GAIN_POS_15DB,
    AFE_VAD_GAIN_POS_18DB,
    AFE_VAD_GAIN_POS_21DB,

    AFE_VAD_GAIN_POS_24DB = 16,
    AFE_VAD_GAIN_POS_27DB,
    AFE_VAD_GAIN_POS_30DB,
    AFE_VAD_GAIN_POS_33DB,
    AFE_VAD_GAIN_POS_36DB,
    AFE_VAD_GAIN_POS_39DB,
    AFE_VAD_GAIN_POS_42DB,
    AFE_VAD_GAIN_POS_45DB,
    AFE_VAD_GAIN_POS_48DB,

} afe_vad_amp_gain_t;


typedef struct dsp_afe_vad_status_s
{
    U8              input_sel;                  /**< @Value 0           @Desc VAD mic selection au_vad_input_select_t */
    U8              amp_gain;                   /**< @Value 0x10        @Desc VAD amp gain afe_vad_amp_gain_t */
    U16             threshold_0;                /**< @Value 7           @Desc VAD Threshold_0 */
    U16             threshold_1;                /**< @Value 0x0200       @Desc VAD Threshold_1 */

    U16             _reserved0;                 /**< @Value 0           @Desc VAD _reserved0 */
    U32             _reserved1;                 /**< @Value 0           @Desc VAD _reserved1 */
} PACKED dsp_afe_vad_status_t;
/******************************************************************************
 * Constants
 ******************************************************************************/


/******************************************************************************
 * DSP Command Structure
 ******************************************************************************/
typedef struct {
    hal_audio_device_parameter_vad_t    vad_parameter;
    SOURCE_TYPE                         source_type;

    //OS_STRU_SEMAPHORE   semaphore;
    OS_STRU_SEMAPHORE_PTR semaphore_ptr;

    bool                                pause_audio_source;
    bool                                enable;
} dsp_vad_control_t;


/******************************************************************************
 * External Global Variables
 ******************************************************************************/


/******************************************************************************
 * External Functions
 ******************************************************************************/
void    dsp_vad_init                (void);
void    dsp_vad_enable              (bool enable, bool suspend_isr);
bool    dsp_vad_get_status          (void);
void    dsp_vad_detect_handler      (void);

#endif /* _DSP_VAD_H_ */

