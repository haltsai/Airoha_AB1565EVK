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

#ifndef _SW_SRC_INTERFACE_H_
#define _SW_SRC_INTERFACE_H_

#if defined(AIR_SOFTWARE_SRC_ENABLE)

/* Includes ------------------------------------------------------------------*/
#include "dsp_utilities.h"
#include "dsp_buffer.h"
#include "dsp_feature_interface.h"
#include "dsp_callback.h"
#include "dsp_sdk.h"

/* Public define -------------------------------------------------------------*/
#define SW_SRC_PORT_MAX         3

/* Public typedef ------------------------------------------------------------*/
typedef enum {
    BLISRC_IN_Q1P15_OUT_Q1P15_I = 0,   // 16-bit Q1.15 input, 16-bit Q1.15 output
    BLISRC_IN_Q1P15_OUT_Q1P31_I = 1,   // 16-bit Q1.15 input, 32-bit Q1.31 output
    BLISRC_IN_Q9P23_OUT_Q1P31_I = 2,   // 32-bit Q8.24 input, 32-bit Q1.31 output
    BLISRC_IN_Q1P31_OUT_Q1P31_I = 3,   // 32-bit Q1.31 input, 32-bit Q1.31 output
} BLISRC_PCM_FORMAT_INTL;

typedef enum {
    SW_SRC_MODE_NORMAL       = 1,
} SW_SRC_MODE;

typedef enum
{
    SW_SRC_PORT_STATUS_DEINIT = 0,
    SW_SRC_PORT_STATUS_INIT = 1,
    SW_SRC_PORT_STATUS_RUNNING = 2
} sw_src_port_status_t;

typedef void Blisrc_Handle;

typedef struct {
    unsigned int in_sampling_rate;
    unsigned int in_channel;
    unsigned int out_sampling_rate;
    unsigned int out_channel;
    unsigned int PCM_Format;
} Blisrc_Param;

typedef struct {
    SW_SRC_MODE mode;
    uint32_t channel_num;
    stream_resolution_t in_res;
    uint32_t in_sampling_rate;
    uint32_t in_frame_size_max;
    stream_resolution_t out_res;
    uint32_t out_sampling_rate;
    uint32_t out_frame_size_max;
} sw_src_config_t;

typedef struct {
    sw_src_port_status_t status;
    void *owner;
    DSP_STREAMING_PARA_PTR stream;
    SW_SRC_MODE mode;
    uint32_t channel_num;
    stream_resolution_t in_res;
    uint32_t in_sampling_rate;
    uint32_t in_frame_size_max;
    stream_resolution_t out_res;
    uint32_t out_sampling_rate;
    uint32_t out_frame_size_max;
    Blisrc_Param blisrc_param;
    bool first_time_flag;
    unsigned int internal_buf_size;
    void *internal_buf_ptr;
    unsigned int temp_buf_size;
    void *temp_buf_ptr;
    uint32_t work_mem_size;
    void *work_mem_ptr;
} sw_src_port_t;

/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
extern int Blisrc_GetBufferSize(unsigned int *p_internal_buf_size_in_byte,
                                unsigned int *p_temp_buf_size_in_byte,
                                Blisrc_Param *p_param);
extern int Blisrc_Open( Blisrc_Handle **pp_handle,
                        void *p_internal_buf,
                        Blisrc_Param *p_param);
extern int Blisrc_Process(  Blisrc_Handle *p_handle,
                            char *p_temp_buf,
                            void *p_in_buf,
                            unsigned int *p_in_byte_cnt,
                            void *p_ou_buf,
                            unsigned int *p_ou_byte_cnt);
extern int Blisrc_Reset(Blisrc_Handle *p_handle);
extern sw_src_port_t *stream_function_sw_src_get_port(void *owner);
extern void stream_function_sw_src_init(sw_src_port_t *port, sw_src_config_t *config);
extern void stream_function_sw_src_deinit(sw_src_port_t *port);
extern bool stream_function_sw_src_initialize(void *para);
extern bool stream_function_sw_src_process(void *para);

#endif /* AIR_SOFTWARE_SRC_ENABLE */

#endif /* _SW_SRC_INTERFACE_H_ */
