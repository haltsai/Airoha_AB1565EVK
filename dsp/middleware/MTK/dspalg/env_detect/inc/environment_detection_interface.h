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

#ifndef _ENVIRONMENT_DETECTION_INTERFACE_H_
#define _ENVIRONMENT_DETECTION_INTERFACE_H_

/* Includes ------------------------------------------------------------------*/
#include "dsp_utilities.h"
#include "dsp_buffer.h"
#include "dsp_feature_interface.h"
#include "dsp_callback.h"
#include "dsp_sdk.h"
#include <stdint.h>
#include "dsp_audio_msg_define.h"


#include "dsp_para_environment_detection.h"
#ifdef AIR_ENVIRONMENT_DETECTION_USE_PIC
#include "environment_detection_portable.h"
#endif

/* Public define -------------------------------------------------------------*/
#define DSP_ENVIRONMENT_DETECTION_BUFFERINF_SIZE   1024


/* Public typedef ------------------------------------------------------------*/

typedef enum
{
    DSP_ENVIRONMENT_DETECTION_DISABLE,
    DSP_ENVIRONMENT_DETECTION_LEVEL_1,
    DSP_ENVIRONMENT_DETECTION_LEVEL_2,
    DSP_ENVIRONMENT_DETECTION_LEVEL_3,
    DSP_ENVIRONMENT_DETECTION_LEVEL_4,
    DSP_ENVIRONMENT_DETECTION_INVALID,
} environment_detection_status_t;

typedef struct
{
    S16 buffer[DSP_ENVIRONMENT_DETECTION_BUFFERINF_SIZE];
    U32 buffer_cnt;
} environment_detection_buf_ctrl;

typedef struct stru_environment_detection_detect_para_u
{
    U32 memory_check;
    U16 process_cnt;
    S16 stationary_noise;
    environment_detection_status_t current_noise_level;
    environment_detection_status_t previous_noise_level;
    U16 pause_procedure;
    U16 consecutive_times;
    U8 nvkey_ready;
    U8 init_done;
    environment_detection_buf_ctrl buf_ctrl;
    DSP_PARA_ENVIRONMENT_DETECTION_STRU nvkey;
    DSP_ALIGN8 U32 scratch_memory;
} ENVIRONMENT_DETECTION_INSTANCE, *ENVIRONMENT_DETECTION_INSTANCE_PTR;


/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
bool stream_function_environment_detection_load_nvkey(void *nvkey);
bool stream_function_environment_detection_get_status(void);
bool stream_function_environment_detection_reset_status(void);
bool stream_function_environment_detection_pause_attenuation(bool is_pause);
void stream_function_environment_detection_set_value(audio_anc_monitor_set_info_t set_type, uint32_t data);
uint32_t stream_function_environment_detection_get_value(audio_anc_monitor_get_info_t get_type);

/**
 * @brief  This function is used to initialize the environment_detection detection in stream flow.
 *
 * @param para is the input parameters.
 * @return true means there is a error.
 * @return false means there is no error.
 */
bool stream_function_environment_detection_initialize(void *para);

/**
 * @brief This function is used to do the environment_detection detection process in stream flow.
 *
 * @param para is the input parameters.
 * @return true means there is a error.
 * @return false means there is no error.
 */
bool stream_function_environment_detection_process(void *para);
#ifndef AIR_ENVIRONMENT_DETECTION_DETECTION_USE_PIC
int get_ne_memsize (void);
void Noise_Est_init (void *handle, void *p_nvkey);
int Noise_Est_Prcs (void *handle, S16 *buf);
#endif
#endif /* _ENVIRONMENT_DETECTION_INTERFACE_H_ */
