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

#ifndef _SW_BUFFER_INTERFACE_H_
#define _SW_BUFFER_INTERFACE_H_

/* Includes ------------------------------------------------------------------*/
#include "dsp_utilities.h"
#include "dsp_buffer.h"
#include "dsp_feature_interface.h"
#include "dsp_callback.h"
#include "dsp_sdk.h"
#include <stdint.h>

/* Public define -------------------------------------------------------------*/
#define SW_BUFFER_PORT_MAX            6
#define SW_BUFFER_LIST_MAX            3

/* Public typedef ------------------------------------------------------------*/
typedef enum
{
    SW_BUFFER_STATUS_ERROR = -1,
    SW_BUFFER_STATUS_OK = 0
} sw_buffer_status_t;

typedef enum
{
    SW_BUFFER_PORT_STATUS_DEINIT = 0,
    SW_BUFFER_PORT_STATUS_INIT = 1,
    SW_BUFFER_PORT_STATUS_RUNNING = 2
} sw_buffer_port_status_t;

typedef enum
{
    SW_BUFFER_MODE_FIXED_OUTPUT_LENGTH,
    SW_BUFFER_MODE_MULTI_BUFFERS,
    SW_BUFFER_MODE_END
} sw_buffer_mode_t;

typedef struct {
    uint32_t buf_size;
    uint32_t data_size;
    uint8_t *buf;
    uint8_t *read_from;
    uint8_t *write_to;
} sw_buffer_channel_t;

typedef struct {
    sw_buffer_port_status_t status;
    void *owner;
    DSP_STREAMING_PARA_PTR stream;
    sw_buffer_mode_t mode;
    uint32_t finish_gpt_count;
    uint16_t watermark_max_size;
    uint16_t watermark_min_size;
    uint16_t output_size;
    uint16_t total_channels;
    sw_buffer_channel_t *channel;
} sw_buffer_port_t;

typedef struct {
    void *owner;
    uint16_t current_index;
    uint16_t buffer_total;
    sw_buffer_port_t **port_list;
} sw_buffer_list_t;

typedef struct {
    /* sw buffer mode: only support SW_BUFFER_MODE_FIXED_OUTPUT_LENGTH at now. */
    sw_buffer_mode_t mode;
    /* The total channel numbers of the sw buffer. For example if the data is stereo, the total_channels should be 2. */
    uint16_t total_channels;
    /* The max watermark and the total buffer size of each channel.
        For example if the watermark_max_size is 1024, it means the buffer size of each channel is 1024B and at most 1024B data can be queued in the channel's buffer. */
    uint16_t watermark_max_size;
    /* The min watermark of each channel.
        For example if the watermark_max_size is 10, it means the sw buffer will output data till there is at least 10B data in the channel's buffer. */
    uint16_t watermark_min_size;
    /* The output data size of each channel.
        For example if the output_size is 2, it means the sw buffer will output 2B data in every stream process */
    uint16_t output_size;
} sw_buffer_config_t;

/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
/**
 * @brief This function is used to get the sw buffer port or query the owner's sw buffer port.
 *        If the owner does not have a sw buffer port, it will malloc a port for this owner.
 *        If the owner have a sw buffer port, it will return the port directly.
 *        If the owner is NULL, it will return the first unused port.
 *
 * @param owner is who want to get or query a sw buffer port or NULL.
 * @return sw_buffer_port_t* is the result.
 */
extern sw_buffer_port_t *stream_function_sw_buffer_get_port(void *owner);

/**
 * @brief This function is used to get the sw buffer port.
 *        If the owner does not have a sw buffer port, it will malloc a port for this owner.
 *        If the owner have a sw buffer port, it also will malloc a port for this owner.
 *        If the owner is NULL, it will return the first unused port.
 *
 * @param owner is who want to get a sw buffer port or NULL.
 * @return sw_buffer_port_t* is the result.
 */
extern sw_buffer_port_t *stream_function_sw_buffer_get_unused_port(void *owner);

/**
 * @brief This function is used to initialize the sw buffer port.
 *
 * @param port is the port which needs to be initialized.
 * @param config is the default configurations of the sw buffer port.
 * @return sw_buffer_status_t is operation result. SW_BUFFER_STATUS_ERROR means there is a error. SW_BUFFER_STATUS_OK means operation is done successfully.
 */
extern sw_buffer_status_t stream_function_sw_buffer_init(sw_buffer_port_t *port, sw_buffer_config_t *config);

/**
 * @brief This function is used to de-initialize the sw buffer port.
 *
 * @param port is the port which needs to be de-initialized.
 * @return sw_buffer_status_t is operation result. SW_BUFFER_STATUS_ERROR means there is a error. SW_BUFFER_STATUS_OK means operation is done successfully.
 */
extern sw_buffer_status_t stream_function_sw_buffer_deinit(sw_buffer_port_t *port);

/**
 * @brief This function is used to query the channel's used space size of the sw buffer port.
 *
 * @param port is the port which needs to be queried.
 * @param channel_number is the channel whose used space size needs to be queried.
 * @return uint32_t is the used space size.
 */
extern uint32_t stream_function_sw_buffer_get_channel_used_size(sw_buffer_port_t *port, uint16_t channel_number);

/**
 * @brief This function is used to query the channel's free space size of the sw buffer port.
 *
 * @param port is the port which needs to be queried.
 * @param channel_number is the channel whose free space size needs to be queried.
 * @return uint32_t is the free space size.
 */
extern uint32_t stream_function_sw_buffer_get_channel_free_size(sw_buffer_port_t *port, uint16_t channel_number);

/**
 * @brief This function is used to get the current read pointer of the sw buffer port.
 *
 * @param port is the port which needs to be queried.
 * @param channel_number is the channel whose read pointer to be queried.
 * @return uint8_t* is pointer to the current read pointer.
 */
extern uint8_t *stream_function_sw_buffer_get_channel_read_pointer(sw_buffer_port_t *port, uint16_t channel_number);

/**
 * @brief This function is used to get the current write pointer of the sw buffer port.
 *
 * @param port is the port which needs to be queried.
 * @param channel_number is the channel whose write pointer to be queried.
 * @return uint8_t* is pointer to the current write pointer.
 */
extern uint8_t *stream_function_sw_buffer_get_channel_write_pointer(sw_buffer_port_t *port, uint16_t channel_number);

/**
 * @brief This function is used to get the start address pointer of the sw buffer port.
 *
 * @param port is the port which needs to be queried.
 * @param channel_number is the channel whose the start address pointer to be queried.
 * @return uint8_t* is pointer to the start address pointer.
 */
extern uint8_t *stream_function_sw_buffer_get_channel_start_pointer(sw_buffer_port_t *port, uint16_t channel_number);

/**
 * @brief This function is used to get the end address pointer of the sw buffer port.
 *
 * @param port is the port which needs to be queried.
 * @param channel_number is the channel whose the end address pointer to be queried.
 * @return uint8_t* is pointer to the end address pointer.
 */
extern uint8_t *stream_function_sw_buffer_get_channel_end_pointer(sw_buffer_port_t *port, uint16_t channel_number);

/**
 * @brief This function is used to configurate the channel's output size of the sw buffer port.
 *
 * @param port is the port which needs to be configurated.
 * @param channel_number is the channel whose output size needs to be configurated.
 * @param new_output_size is the new output size value.
 * @return sw_buffer_status_t is operation result. SW_BUFFER_STATUS_ERROR means there is a error. SW_BUFFER_STATUS_OK means operation is done successfully.
 */
extern sw_buffer_status_t stream_function_sw_buffer_config_channel_output_size(sw_buffer_port_t *port, uint16_t channel_number, uint16_t new_output_size);

/**
 * @brief This function is used to configurate the channel's prefill size of the sw buffer port.
 *
 * @param port is the port which needs to be configurated.
 * @param channel_number is the channel whose prefill size needs to be configurated.
 * @param perfill_size is the prefill size value.
 * @return sw_buffer_status_t is operation result. SW_BUFFER_STATUS_ERROR means there is a error. SW_BUFFER_STATUS_OK means operation is done successfully.
 */
extern sw_buffer_status_t stream_function_sw_buffer_config_channel_prefill_size(sw_buffer_port_t *port, uint16_t channel_number, uint16_t perfill_size, bool set_zeros);

/**
 * @brief This function is used to reset the channel's buffer status of the sw buffer port.
 *
 * @param port is the port which needs to be reset.
 * @param channel_number is the channel whose buffer status is reset.
 * @param set_zeros is the option which if needs to zero the sw buffer. If it is true, the data in the sw buffer will be set 0 after the buffer is reseted.
 * @return sw_buffer_status_t is operation result. SW_BUFFER_STATUS_ERROR means there is a error. SW_BUFFER_STATUS_OK means operation is done successfully.
 */
extern sw_buffer_status_t stream_function_sw_buffer_reset_channel_buffer(sw_buffer_port_t *port, uint16_t channel_number, bool set_zeros);

/**
 * @brief This function is used to get the multi-buffers list.
 *        If the owner does not have a multi-buffers list, it will malloc a list for this owner.
 *        If the owner have a multi-buffers list, it will return the list directly.
 *        If the owner is NULL, it will return the first unused multi-buffers list.
 *
 * @param owner is who want to get or query a multi-buffe list or NULL.
 * @return sw_buffer_list_t* is the result.
 */
extern sw_buffer_list_t *stream_function_sw_buffer_get_list(void *owner);

/**
 * @brief This function is used to initialize the multi-buffers list.
 *
 * @param list is who needs to be initialized.
 * @param buffer_total_number means how much sw buffers will be inserted into the multi-buffers list.
 * @return sw_buffer_status_t is operation result. SW_BUFFER_STATUS_ERROR means there is a error. SW_BUFFER_STATUS_OK means operation is done successfully.
 */
extern sw_buffer_status_t stream_function_sw_buffer_list_init(sw_buffer_list_t *list, uint16_t buffer_total_number);

/**
 * @brief This function is used to de-initialize the multi-buffers list.
 *
 * @param list is who needs to be de-initialized.
 * @return sw_buffer_status_t is operation result. SW_BUFFER_STATUS_ERROR means there is a error. SW_BUFFER_STATUS_OK means operation is done successfully.
 */
extern sw_buffer_status_t stream_function_sw_buffer_list_deinit(sw_buffer_list_t *list);

/**
 * @brief This function is used to insert a specail sw buffer into the multi-buffers list.
 *
 * @param list is the multi-buffers list who will be insert by a specail sw buffer.
 * @param port is the sw buffer port who will be insert into a multi-buffers list.
 * @param buffer_index is the index in the multi-buffers list. For example, buffer_index is 0 means the specail sw buffer will be the first buffer item in the multi-buffers list.
 * @return sw_buffer_status_t is operation result. SW_BUFFER_STATUS_ERROR means there is a error. SW_BUFFER_STATUS_OK means operation is done successfully.
 */
extern sw_buffer_status_t stream_function_sw_buffer_list_insert_buffer(sw_buffer_list_t *list, sw_buffer_port_t *port, uint16_t buffer_index);

/**
 * @brief  This function is used to initialize the sw buffer run-time environment in stream flow.
 *
 * @param para is the input parameters.
 * @return true means there is a error.
 * @return false means there is no error.
 */
extern bool stream_function_sw_buffer_initialize(void *para);

/**
 * @brief This function is used to do the sw buffer process in stream flow.
 *
 * @param para is the input parameters.
 * @return true means there is a error.
 * @return false means there is no error.
 */
extern bool stream_function_sw_buffer_process(void *para);

#endif /* _SW_BUFFER_INTERFACE_H_ */
