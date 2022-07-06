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

#ifndef _SW_GAIN_INTERFACE_H_
#define _SW_GAIN_INTERFACE_H_

/* Includes ------------------------------------------------------------------*/
#include "dsp_utilities.h"
#include "dsp_buffer.h"
#include "dsp_feature_interface.h"
#include "dsp_callback.h"
#include "dsp_sdk.h"
#include <stdint.h>


/* Public define -------------------------------------------------------------*/
#define SW_GAIN_PORT_MAX            3


/* Public typedef ------------------------------------------------------------*/
typedef enum
{
    SW_GAIN_STATUS_ERROR = 0,
    SW_GAIN_STATUS_OK = 1
} sw_gain_status_t;

typedef enum
{
    SW_GAIN_PORT_STATUS_DEINIT = 0,
    SW_GAIN_PORT_STATUS_INIT = 1,
    SW_GAIN_PORT_STATUS_RUNNING = 2
} sw_gain_port_status_t;

typedef struct {
    stream_resolution_t resolution;
    /* Unit: 0.01dB. For example 3 means 0.03dB */
    int32_t target_gain;
    int32_t current_gain;
    /* Unit: 0.01dB. For example 3 means 0.03dB every increase */
    int32_t up_step;
    /* Unit: sample. For example 48 means increases xx.xx dB(up_step) every 48 samples till current gain are as same as target gain */
    uint32_t up_samples_per_step;
    uint32_t current_up_sample;
    /* Unit: 0.01dB. For example -3 means -0.03dB every decrease */
    int32_t down_step;
    /* Unit: sample. For example 48 means decreases xx.xx dB(up_step) every 48 samples till current gain are as same as target gain */
    uint32_t down_samples_per_step;
    uint32_t current_down_sample;
} sw_gain_config_t;

typedef struct {
    sw_gain_port_status_t status;
    void *owner;
    DSP_STREAMING_PARA_PTR stream;
    uint32_t finish_gpt_count;
    uint16_t total_channels;
    sw_gain_config_t *config;
    sw_gain_config_t *temp_config;
} sw_gain_port_t;


/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
/**
 * @brief This function is used to get the sw gain port.
 *        If the owner does not have a sw gain port, it will malloc a port for this owner.
 *        If the owner have a sw gain port, it will return the port directly.
 *        If the owner is NULL, it will return the first unused port.
 *
 * @param owner is who want to get or query a sw gain port or NULL.
 * @return sw_gain_port_t* is the result.
 */
extern sw_gain_port_t *stream_function_sw_gain_get_port(void *owner);

/**
 * @brief This function is used to initialize the sw gain port.
 *
 * @param port is the port which needs to be initialized.
 * @param total_channels is total channel number of the sw gain port.
 * @param default_config is the default configurations of the sw gain port.
 * @return sw_gain_status_t is operation result. SW_GAIN_STATUS_ERROR means there is a error. SW_GAIN_STATUS_OK means operation is done successfully.
 */
extern sw_gain_status_t stream_function_sw_gain_init(sw_gain_port_t *port, uint16_t total_channels, sw_gain_config_t *default_config);

/**
 * @brief This function is used to de-initialize the sw gain port.
 *
 * @param port is the port which needs to be de-initialized.
 * @return sw_gain_status_t is operation result. SW_GAIN_STATUS_ERROR means there is a error. SW_GAIN_STATUS_OK means operation is done successfully.
 */
extern sw_gain_status_t stream_function_sw_gain_deinit(sw_gain_port_t *port);

/**
 * @brief This function is used to get the channel's configuration of the sw gain port.
 *
 * @param port is the port which needs to be got the configuration.
 * @param channel is the channel whose configuration needs to be got.
 * @param config is pointer to where the configuration saved into.
 * @return sw_gain_status_t is operation result. SW_GAIN_STATUS_ERROR means there is a error. SW_GAIN_STATUS_OK means operation is done successfully.
 */
extern sw_gain_status_t stream_function_sw_gain_get_config(sw_gain_port_t *port, uint16_t channel, sw_gain_config_t *config);

/**
 * @brief This function is used to configurate the channel's resolution of the sw gain port.
 *
 * @param port is the port which needs to be configurated.
 * @param channel is the channel whose resolution needs to be configurated.
 * @param new_resolution is the new resolution value.
 * @return sw_gain_status_t is operation result. SW_GAIN_STATUS_ERROR means there is a error. SW_GAIN_STATUS_OK means operation is done successfully.
 */
extern sw_gain_status_t stream_function_sw_gain_configure_resolution(sw_gain_port_t *port, uint16_t channel, stream_resolution_t new_resolution);

/**
 * @brief This function is used to configurate the channel's target gain of the sw gain port.
 *
 * @param port is the port which needs to be configurated.
 * @param channel is the channel whose target gain needs to be configurated.
 * @param new_gain is the new gain value.
 * @return sw_gain_status_t is operation result. SW_GAIN_STATUS_ERROR means there is a error. SW_GAIN_STATUS_OK means operation is done successfully.
 */
extern sw_gain_status_t stream_function_sw_gain_configure_gain_target(sw_gain_port_t *port, uint16_t channel, int32_t new_gain);

/**
 * @brief This function is used to configurate the channel's gain up settings of the sw gain port.
 *
 * @param port is the port which needs to be configurated.
 * @param channel is the channel whose gain up settings needs to be configurated.
 * @param new_up_step is the new up step value.
 * @param new_up_samples_per_step is the new up sample per step value.
 * @return sw_gain_status_t is operation result. SW_GAIN_STATUS_ERROR means there is a error. SW_GAIN_STATUS_OK means operation is done successfully.
 */
extern sw_gain_status_t stream_function_sw_gain_configure_gain_up(sw_gain_port_t *port, uint16_t channel, int32_t new_up_step, int32_t new_up_samples_per_step);

/**
 * @brief This function is used to configurate the channel's gain down settings of the sw gain port.
 *
 * @param port is the port which needs to be configurated.
 * @param channel is the channel whose gain down settings needs to be configurated.
 * @param new_down_step is the new down step value.
 * @param new_down_samples_per_step is the new down sample per step value.
 * @return sw_gain_status_t is operation result. SW_GAIN_STATUS_ERROR means there is a error. SW_GAIN_STATUS_OK means operation is done successfully.
 */
extern sw_gain_status_t stream_function_sw_gain_configure_gain_down(sw_gain_port_t *port, uint16_t channel, int32_t new_down_step, int32_t new_down_samples_per_step);

/**
 * @brief  This function is used to initialize the sw gain run-time environment in stream flow.
 *
 * @param para is the input parameters.
 * @return true means there is a error.
 * @return false means there is no error.
 */
extern bool stream_function_sw_gain_initialize(void *para);

/**
 * @brief This function is used to do the sw gain process in stream flow.
 *
 * @param para is the input parameters.
 * @return true means there is a error.
 * @return false means there is no error.
 */
extern bool stream_function_sw_gain_process(void *para);

#endif /* _SW_GAIN_INTERFACE_H_ */
