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

#ifndef _SW_MIXER_INTERFACE_H_
#define _SW_MIXER_INTERFACE_H_

/* Includes ------------------------------------------------------------------*/
#include "dsp_utilities.h"
#include "dsp_buffer.h"
#include "dsp_feature_interface.h"
#include "dsp_callback.h"
#include "dsp_sdk.h"
#include <stdint.h>


/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef enum
{
    SW_MIXER_STATUS_ERROR = -1,
    SW_MIXER_STATUS_OK = 0
} sw_mixer_status_t;

typedef enum
{
    SW_MIXER_PORT_0 = 0,
    SW_MIXER_PORT_MAX
} sw_mixer_port_t;

typedef enum
{
    SW_MIXER_PORT_STATUS_DEINIT = 0,
    SW_MIXER_PORT_STATUS_INIT = 1
} sw_mixer_port_status_t;

typedef enum
{
    SW_MIXER_MEMBER_STATUS_DELETED = 0,
    SW_MIXER_MEMBER_STATUS_CREATED = 1,
    SW_MIXER_MEMBER_STATUS_REGISTERED = 2,
    SW_MIXER_MEMBER_STATUS_UNREGISTERED = 3
} sw_mixer_member_status_t;

typedef enum
{
    /* all source operation and sink operation and features are run. */
    SW_MIXER_MEMBER_MODE_NO_BYPASS,
    /* all source operation and sink operation are run, but all subsequent features will be bypassed. */
    SW_MIXER_MEMBER_MODE_BYPASS_FEATURES,
    /* all source operation are run, but all subsequent features and sink operation will be bypassed. */
    SW_MIXER_MEMBER_MODE_BYPASS_FEATURES_SINK,
    /* all source operation and sink operation and features are bypassed. */
    SW_MIXER_MEMBER_MODE_BYPASS_ALL,
    SW_MIXER_MEMBER_MODE_MAX
} sw_mixer_member_mode_t;

typedef enum
{
    /* If empty space is less than new data length, the new data will overrun the oldest data. */
    SW_MIXER_CHANNEL_MODE_NORMAL,
    /* The new data always overwrite the internal buffer (read pointer is fixed). If new data length is larger than internal buffer total length, the mixer will trigger assert. */
    SW_MIXER_CHANNEL_MODE_OVERWRITE,
    /* If empty space is less than new data length, the mixer will trigger assert. */
    SW_MIXER_CHANNEL_MODE_FORCED_ENOUGH,
    SW_MIXER_CHANNEL_MODE_MAX
} sw_mixer_channel_mode_t;

typedef enum
{
    /* The output length is the data length of the Main input channel. */
    SW_MIXER_CHANNEL_ATTRIBUTE_MAIN,
    /* If the data length of any Forced_wait input channel is less than the output length, the mixer will be passed and set output length to zero. */
    /* If the data length of all Forced_wait input channel is equal or greater than the output length, the mixer will only fetch the output length data from the Forced_wait input channel. */
    SW_MIXER_CHANNEL_ATTRIBUTE_FORCED_WAIT,
    /* If there is data in any Normal input channel, the mixer will fetch data from this normal input channel. */
    /* If the data length in this normal input channel is less than the output length, the mixer will fetch all data in normal input channel. */
    /* If the data length in normal input channel is larger than the output length, the mixer will fetch the output length data in normal input channel. */
    SW_MIXER_CHANNEL_ATTRIBUTE_NORMAL,
    /* If there is data in any Normal_wait input channel and data length is equal or greater than the output length, the mixer will fetch the output length data from the normal_wait input channel. */
    /* If the data length is less than the output length, the mixer will bypass this channel. */
    SW_MIXER_CHANNEL_ATTRIBUTE_NORMAL_WAIT,
    SW_MIXER_CHANNEL_ATTRIBUTE_MAX
} sw_mixer_channel_attribute_t;

typedef struct sw_mixer_list_item_t sw_mixer_list_item_t;

struct sw_mixer_list_item_t {
    /* pointer to input channel or output channel */
    void *channel;
    /* for input channel, attribute is unmeaning */
    /* for output channel, attribute means the rules of mixing data */
    sw_mixer_channel_attribute_t attribute;
    /* pointer to the next item on this list */
    sw_mixer_list_item_t *next_item;
    /* pointer to the pervious item on this list */
    sw_mixer_list_item_t *perv_item;
};

typedef struct {
    /* total number of the items on this list */
    uint32_t item_number;
    /* pointer to the first item on this list */
    sw_mixer_list_item_t *first_item;
    /* pointer to the last item on this list */
    sw_mixer_list_item_t *last_item;
} sw_mixer_list_t;

typedef struct sw_mixer_member_t sw_mixer_member_t;
typedef struct sw_mixer_input_channel_t sw_mixer_input_channel_t;
typedef struct sw_mixer_output_channel_t sw_mixer_output_channel_t;

typedef void (*sw_mixer_preprocess_callback_t)(sw_mixer_member_t *member, void *para);
typedef void (*sw_mixer_postprocess_callback_t)(sw_mixer_member_t *member, void *para, uint32_t *out_frame_size);

typedef struct {
    sw_mixer_port_status_t status;
    uint16_t count;
    uint16_t total_members;
    sw_mixer_member_t *first_member;
    sw_mixer_member_t *last_member;
} sw_mixer_port_handle_t;

struct sw_mixer_member_t {
    sw_mixer_member_status_t status;
    void *owner;
    DSP_STREAMING_PARA_PTR stream;
    uint32_t finish_gpt_count;
    sw_mixer_port_t port;
    int32_t number;
    sw_mixer_member_mode_t mode;
    sw_mixer_preprocess_callback_t preprocess_callback;
    sw_mixer_postprocess_callback_t postprocess_callback;
    uint32_t default_connected;
    uint16_t total_in_channels;
    uint16_t total_out_channels;
    sw_mixer_input_channel_t  *input_ch;
    sw_mixer_output_channel_t *output_ch;
    sw_mixer_member_t *next_member;
    sw_mixer_member_t *perv_member;
};

typedef struct {
    sw_mixer_preprocess_callback_t preprocess_callback;
    sw_mixer_postprocess_callback_t postprocess_callback;
} sw_mixer_callback_config_t;

struct sw_mixer_input_channel_t {
    sw_mixer_member_t *member;
    int32_t number;
    stream_resolution_t resolution;
    sw_mixer_channel_mode_t input_mode;
    uint32_t buf_size;
    uint32_t data_size;
    uint8_t *in_buf;
    uint8_t *read_from;
    uint8_t *write_to;
    /* The items on the out_channel_list are
       all output channels which are bound to this input channel */
    sw_mixer_list_t out_channel_list;
};

typedef struct {
    uint32_t total_channel_number;
    stream_resolution_t resolution;
    sw_mixer_channel_mode_t input_mode;
    uint32_t buffer_size;
} sw_mixer_input_channel_config_t;

struct sw_mixer_output_channel_t {
    sw_mixer_member_t *member;
    int32_t number;
    stream_resolution_t resolution;
    uint32_t data_size;
    uint8_t *out_buf;
    /* The items on the in_channel_list are
       all input channels which are bound to this output channel */
    sw_mixer_list_t in_channel_list;
};

typedef struct {
    uint32_t total_channel_number;
    stream_resolution_t resolution;
} sw_mixer_output_channel_config_t;


/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
/**
 * @brief This function is used to initialize the sw mixer port.
 *
 * @param port is the port which needs to be initialized.
 * @return sw_mixer_status_t is operation result. SW_MIXER_STATUS_ERROR means there is a error. SW_MIXER_STATUS_OK means operation is done successfully.
 */
extern sw_mixer_status_t stream_function_sw_mixer_init(sw_mixer_port_t port);

/**
 * @brief This function is used to de-initialize the sw mixer port.
 *
 * @param port is the port which needs to be de-initialized.
 * @return sw_mixer_status_t is operation result. SW_MIXER_STATUS_ERROR means there is a error. SW_MIXER_STATUS_OK means operation is done successfully.
 */
extern sw_mixer_status_t stream_function_sw_mixer_deinit(sw_mixer_port_t port);

/**
 * @brief This function is used to create the sw mixer member.
 *
 * @param owner is who want to get a sw mixer member.
 * @param mode is sw mixer flow mode.
 * @param callback_config is used for registering pre-callback and post-callback.
 * @param in_ch_config is used for input channels' configurations.
 * @param out_ch_config is used for input channels' configurations.
 * @return sw_mixer_member_t* is the new created member.
 */
extern sw_mixer_member_t *stream_function_sw_mixer_member_create(void *owner, sw_mixer_member_mode_t mode, sw_mixer_callback_config_t *callback_config, sw_mixer_input_channel_config_t *in_ch_config, sw_mixer_output_channel_config_t *out_ch_config);

/**
 * @brief This function is used to register a sw mixer member into a sw port.
 *
 * @param port is the port which needs to be registered a new member.
 * @param member is the member who needs to be registered.
 * @param default_connected is default connection setting. If it is true, the input channel will be connected to the output channel in a pair.
 * @return sw_mixer_status_t is operation result. SW_MIXER_STATUS_ERROR means there is a error. SW_MIXER_STATUS_OK means operation is done successfully.
 */
extern sw_mixer_status_t stream_function_sw_mixer_member_register(sw_mixer_port_t port, sw_mixer_member_t *member, bool default_connected);

/**
 * @brief This function is used to unregister a sw mixer member into a sw port.
 *
 * @param port is the port which needs to be unregistered a member.
 * @param member is the member who needs to be unregistered.
 * @return sw_mixer_status_t is operation result. SW_MIXER_STATUS_ERROR means there is a error. SW_MIXER_STATUS_OK means operation is done successfully.
 */
extern sw_mixer_status_t stream_function_sw_mixer_member_unregister(sw_mixer_port_t port, sw_mixer_member_t *member);

/**
 * @brief This function is used to deleted the sw mixer member.
 *
 * @param member is the member who needs to be deleted.
 * @return sw_mixer_status_t is operation result. SW_MIXER_STATUS_ERROR means there is a error. SW_MIXER_STATUS_OK means operation is done successfully.
 */
extern sw_mixer_status_t stream_function_sw_mixer_member_delete(sw_mixer_member_t *member);

/**
 * @brief This function is used to reset all input channels' buffer.
 *
 * @param member is the member whose all input channels' buffer will be reset.
 * @param set_zeros is set zero flag. If it is true, all input channels' buffer will be zeroed after reset.
 * @return sw_mixer_status_t is operation result. SW_MIXER_STATUS_ERROR means there is a error. SW_MIXER_STATUS_OK means operation is done successfully.
 */
extern sw_mixer_status_t stream_function_sw_mixer_member_input_buffers_clean(sw_mixer_member_t *member, bool set_zeros);

/**
 * @brief This function is used to disconnect all input channels and all output channels of this member.
 *
 * @param member is the member whose all input channel and all output channels are disconnected.
 * @return sw_mixer_status_t is operation result. SW_MIXER_STATUS_ERROR means there is a error. SW_MIXER_STATUS_OK means operation is done successfully.
 */
extern sw_mixer_status_t stream_function_sw_mixer_channel_disconnect_all(sw_mixer_member_t *member);

/**
 * @brief This function is used to connect the input channel to the output channel.
 *
 * @param in_member is the member whose owns the input channel.
 * @param in_ch_number is the input channel who needs to be connected.
 * @param attribute is connection attribute.
 * @param out_member is the member whose owns the output channel.
 * @param out_ch_number is the outputput channel who needs to be connected.
 * @return sw_mixer_status_t is operation result. SW_MIXER_STATUS_ERROR means there is a error. SW_MIXER_STATUS_OK means operation is done successfully.
 */
extern sw_mixer_status_t stream_function_sw_mixer_channel_connect(sw_mixer_member_t *in_member, uint32_t in_ch_number, sw_mixer_channel_attribute_t attribute, sw_mixer_member_t *out_member, uint32_t out_ch_number);

/**
 * @brief This function is used to disconnect the input channel to the output channel.
 *
 * @param in_member is the member whose owns the input channel.
 * @param in_ch_number is the input channel who needs to be connected.
 * @param out_member is the member whose owns the output channel.
 * @param out_ch_number is the outputput channel who needs to be connected.
 * @return sw_mixer_status_t is operation result. SW_MIXER_STATUS_ERROR means there is a error. SW_MIXER_STATUS_OK means operation is done successfully.
 */
extern sw_mixer_status_t stream_function_sw_mixer_channel_disconnect(sw_mixer_member_t *in_member, uint32_t in_ch_number, sw_mixer_member_t *out_member, uint32_t out_ch_number);

/**
 * @brief This function is used to reset a input channel's buffer.
 *
 * @param member is the member whose a input channel's buffer will be reset.
 * @param in_ch_number is the input channel who needs to be reset.
 * @param set_zeros is set zero flag. If it is true, this input channel's buffer will be zeroed after reset.
 * @return sw_mixer_status_t is operation result. SW_MIXER_STATUS_ERROR means there is a error. SW_MIXER_STATUS_OK means operation is done successfully.
 */
extern sw_mixer_status_t stream_function_sw_mixer_channel_input_buffer_clean(sw_mixer_member_t *member, uint32_t in_ch_number, bool set_zeros);

/**
 * @brief  This function is used to initialize the sw mixer run-time environment in stream flow.
 *
 * @param para is the input parameters.
 * @return true means there is a error.
 * @return false means there is no error.
 */
extern bool stream_function_sw_mixer_initialize(void *para);

/**
 * @brief This function is used to do the sw mixer process in stream flow.
 *
 * @param para is the input parameters.
 * @return true means there is a error.
 * @return false means there is no error.
 */
extern bool stream_function_sw_mixer_process(void *para);

#endif /* _SW_MIXER_INTERFACE_H_ */
