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

#ifndef _WIND_INTERFACE_H_
#define _WIND_INTERFACE_H_

/* Includes ------------------------------------------------------------------*/
#include "dsp_utilities.h"
#include "dsp_buffer.h"
#include "dsp_feature_interface.h"
#include "dsp_callback.h"
#include "dsp_sdk.h"
#include <stdint.h>

#include "dsp_para_wind.h"
#ifdef AIR_WIND_DETECTION_USE_PIC
#include "wind_portable.h"
#endif

/* Public define -------------------------------------------------------------*/
#define DSP_WIND_DETECTION_BUFFERINF_SIZE   1024


/* Public typedef ------------------------------------------------------------*/
typedef struct
{
    S16 buffer[DSP_WIND_DETECTION_BUFFERINF_SIZE];
    U32 buffer_cnt;
} wind_buf_ctrl;

typedef struct stru_wind_detect_para_u
{
    U32 memory_check;
    U32 process_cnt;
    S16 detect_wind;
    U16 detection_period;
    U16 consecutive_times;
    U8 nvkey_ready;
    U8 init_done;
    wind_buf_ctrl buf_ctrl;
    DSP_PARA_WIND_STRU nvkey;
    DSP_ALIGN8 U32 scratch_memory;
} WIND_INSTANCE, *WIND_INSTANCE_PTR;


/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
bool stream_function_wind_load_nvkey(void *nvkey);
bool stream_function_wind_get_status(void);
bool stream_function_wind_reset_status(void);

/**
 * @brief  This function is used to initialize the wind detection in stream flow.
 *
 * @param para is the input parameters.
 * @return true means there is a error.
 * @return false means there is no error.
 */
bool stream_function_wind_initialize(void *para);

/**
 * @brief This function is used to do the wind detection process in stream flow.
 *
 * @param para is the input parameters.
 * @return true means there is a error.
 * @return false means there is no error.
 */
bool stream_function_wind_process(void *para);
#ifndef AIR_WIND_DETECTION_USE_PIC
int WindDet_get_memsize(void);
int WindDet_Init(void *fix_rnn_t, void *nvkey_t);
int WindDet_Prcs(void *fix_rnn_t, short *datain, short *fflag);
#endif
#endif /* _WIND_INTERFACE_H_ */
