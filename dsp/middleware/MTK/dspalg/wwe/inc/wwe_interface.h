/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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

#ifndef _WWE_H_
#define _WWE_H_

#ifdef MTK_WWE_ENABLE

#include "dsp_feature_interface.h"
#include "dsp_utilities.h"
#include "common.h"
#include "hwvad.h"
#include "wwe_mem.h"
#include "preroll.h"
#include "dsp_nvkey_vad_comm.h"
#include "dsp_nvkey_vad_para.h"
#include "dsp_nvkey_vow_para.h"

typedef struct {
    DSP_NVKEY_VAD_COMM vad_nvkey_common;
    DSP_NVKEY_VAD_PARA vad_nvkey_1mic_v_mode;
    DSP_NVKEY_VAD_PARA vad_nvkey_1mic_c_mode;
    DSP_NVKEY_VAD_PARA vad_nvkey_2mic_v_mode;
    DSP_NVKEY_VAD_PARA vad_nvkey_2mic_c_mode;
    uint32_t language_mode_address;
    uint32_t language_mode_length;
    DSP_NVKEY_VOW_PARA vow_setting;
    uint8_t adda_analog_mic_mode; /*0x0: ACC_10k, 0x1: ACC_20k,0x2: DCC*/
} mcu2dsp_vad_param_t, *mcu2dsp_vad_param_p;

//Define for forward frame number
#define FORWARD_FRAME_NUMBER           14

typedef enum {
    WWE_MODE_NONE =    0,
    WWE_MODE_AMA =     1,
    WWE_MODE_GSOUND =   2,
    WWE_MODE_VENDOR1 = 3,
    WWE_MODE_MAX
} wwe_mode_t;

typedef enum {
    WWE_STATE_NONE = 0,
    WWE_STATE_HWVAD = 1,
    WWE_STATE_LBF = 2,
    WWE_STATE_WWD   = 3
} wwe_state_t;

extern volatile wwe_state_t g_wwe_state;
extern volatile wwe_mode_t g_wwe_mode;
extern volatile uint32_t g_wwe_frame_size;
extern volatile bool g_is_wwe_success;
extern volatile mcu2dsp_vad_param_p g_mcu2dsp_vad_param;

#define WWE_PREPROC_MEMSIZE 0x0000
#define WWE_PROC_MEMSIZE    0x0000


EXTERN bool stream_function_wwe_preprocessing_initialize(void *para);
EXTERN bool stream_function_wwe_preprocessing_process(void *para);
EXTERN bool stream_function_wwe_processing_initialize(void *para);
EXTERN bool stream_function_wwe_processing_process(void *para);

EXTERN VOID wwe_processing_init(void);
EXTERN VOID wwe_processing_deinit(void);

EXTERN VOID wwe_hwvad_resume(void);
EXTERN VOID wwe_hwvad_deinit(void);

#endif

#endif /* _WWE_H_ */
