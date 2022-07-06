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

#ifndef _CLK_SKEW_SW_H_
#define _CLK_SKEW_SW_H_

/* Includes ------------------------------------------------------------------*/
#include "dsp_utilities.h"
#include "dsp_buffer.h"
#include "dsp_feature_interface.h"
#include "dsp_callback.h"
#include "dsp_sdk.h"
#include "skew_ctrl.h"


/* Public define -------------------------------------------------------------*/
#define SW_CLK_SKEW_PORT_MAX            3

#define SW_CLK_SKEW_STATUS_DEINIT       0
#define SW_CLK_SKEW_STATUS_INIT         1
#define SW_CLK_SKEW_STATUS_RUNNING      2


/* Public typedef ------------------------------------------------------------*/
typedef enum {
    SW_CLK_SKEW_CONTINUOUS = 1,
    SW_CLK_SKEW_OCCASIONAL
} sw_clk_skew_work_mode_t;

typedef enum {
    SW_CLK_SKEW_COMPENSATION_1_SAMPLE_IN_1_FRAME = 1,
    SW_CLK_SKEW_COMPENSATION_1_SAMPLE_IN_4_FRAME = 4,
    SW_CLK_SKEW_COMPENSATION_1_SAMPLE_IN_8_FRAME = 8
} sw_clk_skew_compensation_mode_t;

typedef struct {
    U8 *p_internal_buffer;
    skew_ctrl_t skew_ctrl;
    uint32_t last_sample_value;
} sw_clk_skew_channel_config_t;

typedef struct {
    U32 status;
    void *owner;
    DSP_STREAMING_PARA_PTR stream;
    uint32_t finish_gpt_count;
    U16 channel;
    U16 bits;
    U16 order;
    U16 max_output_size;
    skew_mode_t skew_io_mode;
    sw_clk_skew_compensation_mode_t skew_compensation_mode;
    sw_clk_skew_work_mode_t skew_work_mode;
    U16 continuous_frame_size;
    S16 compen_samples_in_each_frame;
    sw_clk_skew_channel_config_t *channel_config;
} sw_clk_skew_port_t;

typedef struct {
    U16 channel;
    U16 bits;
    U16 order;
    U16 max_output_size;
    U16 continuous_frame_size;
    skew_mode_t skew_io_mode;
    sw_clk_skew_compensation_mode_t skew_compensation_mode;
    sw_clk_skew_work_mode_t skew_work_mode;
} sw_clk_skew_config_t;


/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
/**
 * @brief This function is used to get the sw_clk_skew port.
 *        If the owner does not have a sw_clk_skew port, it will malloc a port for this owner.
 *        If the owner have a sw_clk_skew port, it will return the port directly.
 *        If the owner is NULL, it will return the first unused port.
 *
 * @param owner is who want to get or query a sw_clk_skew port or NULL.
 * @return sw_clk_skew_port_t* is the result.
 */
extern sw_clk_skew_port_t *stream_function_sw_clk_skew_get_port(void *owner);

/**
 * @brief This function is used to init the sw_clk_skew port.
 *
 * @param port is which port needs to be inited.
 * @param config is pointer to the configurations.
 */
extern void stream_function_sw_clk_skew_init(sw_clk_skew_port_t *port, sw_clk_skew_config_t *config);

/**
 * @brief This function is used to deinit the sw_clk_skew port.
 *
 * @param port is which port needs to be deinited.
 */
extern void stream_function_sw_clk_skew_deinit(sw_clk_skew_port_t *port);

/**
 * @brief This function is used to set the compensation samples in each frame.
 *
 * @param port is which port needs to be configured.
 * @param compen_samples_in_each_frame is compensation samples.
 */
extern void stream_function_sw_clk_skew_configure_compensation_samples(sw_clk_skew_port_t *port, S32 compen_samples_in_each_frame);

/**
 * @brief This function is used to set the compensation mode.
 *
 * @param port is which port needs to be configured.
 * @param compensation_mode is compensation mode.
 */
extern void stream_function_sw_clk_skew_configure_compensation_mode(sw_clk_skew_port_t *port, sw_clk_skew_compensation_mode_t compensation_mode);

/**
 * @brief This function is used to initialize the sw clk skew run-time environment.
 *
 * @param para is the input parameters.
 * @return true means there is a error.
 * @return false means there is no error.
 */
extern bool stream_function_sw_clk_skew_initialize(void *para);

/**
 * @brief This function is used to do the sw clk skew process.
 *
 * @param para is the input parameters.
 * @return true means there is a error.
 * @return false means there is no error.
 */
extern bool stream_function_sw_clk_skew_process(void *para);

#endif /* _CLK_SKEW_SW_H_ */
