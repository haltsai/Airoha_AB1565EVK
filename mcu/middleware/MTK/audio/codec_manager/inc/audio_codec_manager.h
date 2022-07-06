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

#ifndef __AUDIO_CODEC_MANAGER_H__
#define __AUDIO_CODEC_MANAGER_H__

#ifdef MTK_OPUS_ENCODER_ENABLE
#include "audio_codec_opus_encoder.h"
#endif
#ifdef MTK_SBC_ENCODER_ENABLE
#include "audio_codec_sbc_encoder.h"
#endif
#include "audio_codec_none_codec.h"
#ifdef MTK_WAV_DECODER_ENABLE
#include "audio_codec_wav_decoder.h"
#endif
#ifdef AIR_OPUS_DECODER_ENABLE
#include "audio_codec_opus_decoder.h"
#endif
#ifdef AIR_MP3_DECODER_ENABLE
#include "audio_codec_mp3_decoder.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
 * Enums
 *****************************************************************************/

/** @defgroup audio_codec_manager_enum Enum
 *  @{
 */

/** @brief This enum defines the codec return types*/
typedef enum {
    AUDIO_CODEC_MANAGER_ERROR_PARAM = -3,
    AUDIO_CODEC_MANAGER_ERROR_LEN = -2,
    AUDIO_CODEC_MANAGER_ERROR = -1,
    AUDIO_CODEC_MANAGER_SUCCESS = 0,
    AUDIO_CODEC_MANAGER_STREAM_PROCESS_END = 1, // the eof of stream
} audio_codec_manager_status_t;

/** @brief This enum defines the codec types*/
typedef enum {
    CODEC_TYPE_NONE = 0,
#ifdef MTK_OPUS_ENCODER_ENABLE
    CODEC_TYPE_OPUS_ENCODE,
#endif
#ifdef MTK_SBC_ENCODER_ENABLE
    CODEC_TYPE_SBC_ENCODE,
#endif
#ifdef MTK_WAV_DECODER_ENABLE
    CODEC_TYPE_WAV_DECODE,
#endif
#ifdef AIR_MP3_DECODER_ENABLE
    CODEC_TYPE_MP3_DECODE,
#endif
#ifdef AIR_OPUS_DECODER_ENABLE
    CODEC_TYPE_OPUS_DECODE,
#endif
    CODEC_TYPE_MAX
} audio_codec_manager_type_t;

/** @brief This enum defines the codec mode*/
typedef enum {
    CODEC_MODE_DERECT = 1,
    CODEC_MODE_BUFFER
} codec_mode_t;

/** @brief This enum defines the codec event types*/
typedef enum {
    AUDIO_CODEC_MANAGER_EVENT_OUTPUT_BUFFER_FULL = 1
} audio_codec_manager_event_t;

/**
 * @}
 */

/*****************************************************************************
 * Union
 *****************************************************************************/

/** @defgroup codec_union Union
  * @{
  */

/** @brief This union define the codec config*/
typedef union {
    none_codec_config_t none_codec_config;
#ifdef MTK_OPUS_ENCODER_ENABLE
    opus_encoder_config_t opus_encoder_config;
#endif
#ifdef MTK_SBC_ENCODER_ENABLE
    sbc_encoder_config_t sbc_encoder_config;
#endif
#ifdef MTK_WAV_DECODER_ENABLE
    wav_decoder_config_t wav_decoder_config;
#endif
#ifdef AIR_MP3_DECODER_ENABLE
    mp3_decoder_config_t  mp3_decoder_config;
#endif
#ifdef AIR_OPUS_DECODER_ENABLE
    opus_decoder_config_t opus_decoder_config;
#endif
} codec_config_t;

/**
 * @}
 */

/*****************************************************************************
 * Typedef
 *****************************************************************************/

/** @defgroup audio_codec_manager_typedef Typedef
  * @{
  */

/** @brief This define the codec handle*/
typedef uint32_t audio_codec_manager_handle_t;

/** @brief  This defines the codec callback function prototype.
 *          Register a callback function while codec meet some event.
 *          For more details about the event, please refer to #audio_codec_manager_event_t
 * @param[in] handle       Codec handel.
 *  @param[in] event       The value defined in #audio_codec_manager_event_t. This parameter is given by the codec to notify the user about the data processing behavior.
 *  @param[in] user_data   A user defined parameter.
 */
typedef void (*audio_codec_manager_callback_t)(audio_codec_manager_handle_t handle, audio_codec_manager_event_t event, void *user_data);
/**
 * @}
 */

/*****************************************************************************
 * Structures
 *****************************************************************************/

/** @defgroup audio_codec_manager_struct Struct
 *  @{
 */

/** @brief This struct defines the config informations for codec*/
typedef struct {
    codec_mode_t        codec_mode;
    codec_config_t      codec_config;
} audio_codec_manager_config_t;

/** @brief This struct defines the config informations special for codec buffer mode*/
typedef struct {
    uint8_t *output_buffer;
    uint32_t output_length;
    audio_codec_manager_callback_t callback;
    void *user_data;
} audio_codec_manager_user_config_t;

/**
 * @}
 */

/*****************************************************************************
 * Functions
 *****************************************************************************/

/**
 * @brief     This function is to open codec.
 *               If some spacial type codec(CODEC_TYPE_OPUS_ENCODE/CODEC_TYPE_SBC_ENCODE) had already opened, cannot be open again.
 *
 * @param[in] type       Choose which type of codec.
 * @param[in] *config     Audio config information pass to codec, include codec mode and parameter.
 * @param[out] *handle    If open codec success, user get the codec handle.
 * @return
 * #AUDIO_CODEC_MANAGER_SUCCESS, Codec open success. \n
 * #AUDIO_CODEC_MANAGER_ERROR,   Codec open failed. \n
 */
audio_codec_manager_status_t audio_codec_open(audio_codec_manager_type_t type, audio_codec_manager_config_t *config, audio_codec_manager_handle_t *handle);

/**
 * @brief     This function is to get the size of memory that the codec used for internal algorithm calculate.
 *               User need allocate memory and pass to codec if needed.
 *
 * @param[in] type       Choose which type of codec.
 * @param[in] *config     Audio config information pass to codec, include codec mode and parameter.
 * @param[out] *working_buffer_length    If open codec success, user get the codec handle.
 * @return
 * #AUDIO_CODEC_MANAGER_SUCCESS, Get working buffer length success. \n
 * #AUDIO_CODEC_MANAGER_ERROR,   Get working buffer length failed. \n
 */
audio_codec_manager_status_t audio_codec_get_working_buffer_length(audio_codec_manager_type_t type, audio_codec_manager_config_t *codec_config, uint32_t *working_buffer_length);

/**
 * @brief     This function is to get the size of one frame input/output data.
 *
 * @param[in] handle       Codec handel.
 * @param[out] *oneframe_input_byte_length     Size of one frame input data .
 * @param[out] *oneframe_output_byte_length    Size of one frame output data .
 * @return
 * #AUDIO_CODEC_MANAGER_SUCCESS, Get frame length success. \n
 * #AUDIO_CODEC_MANAGER_ERROR,   Get frame length failed. \n
 *
 */
audio_codec_manager_status_t audio_codec_get_inout_frame_length(audio_codec_manager_handle_t handle, uint32_t *oneframe_input_byte_length, uint32_t *oneframe_output_byte_length);

/**
 * @brief     This function is to do codec process at direct mode.
 *
 * @param[in] handle       Codec handel.
 * @param[in] *input_buffer          Input data`s address.
 * @param[in] *input_byte_length     Input data`s length.
 * @param[out] *input_byte_length    Length of input data that the codec actually uesd.
 * @param[in] *output_buffer         Output data`s address.
 * @param[in] *output_byte_length    Length of output data that user can accept.
 * @param[out] *output_byte_length   Length of output data that the user actually gets.
 *
 * @return
 * #AUDIO_CODEC_MANAGER_SUCCESS, process success. \n
 * #AUDIO_CODEC_MANAGER_ERROR,   process failed. \n
 *
 */
audio_codec_manager_status_t audio_codec_direct_mode_process(audio_codec_manager_handle_t handle, uint8_t *input_buffer, uint32_t *input_byte_length, uint8_t *output_buffer, uint32_t *output_byte_length);
/**
 * @brief     This function is to set codec config at direct mode.
 *            For some codec, user can run time change parameter.
 *
 * @param[in] handle       Codec handel.
 * @param[in] *config       Audio config information pass to codec.
 *
 * @return
 * #AUDIO_CODEC_MANAGER_SUCCESS, process success. \n
 * #AUDIO_CODEC_MANAGER_ERROR,   process failed. \n
 *
 */
audio_codec_manager_status_t audio_codec_direct_mode_set_codec_config(audio_codec_manager_handle_t handle, audio_codec_manager_config_t *codec_config);

/**
 * @brief     This function is to set user config at buffer mode.
 *
 * @param[in] handle            Codec handel.
 * @param[in] *user_config       user config information pass to codec.
 *
 * @return
 * #AUDIO_CODEC_MANAGER_SUCCESS, process success. \n
 * #AUDIO_CODEC_MANAGER_ERROR,   process failed. \n
 *
 */
audio_codec_manager_status_t audio_codec_buffer_mode_set_config(audio_codec_manager_handle_t handle, audio_codec_manager_user_config_t *user_config);

/**
 * @brief     This function is to do codec process at buffer mode.
 *
 * @param[in] handle       Codec handel.
 * @param[in] *input_buffer          Input data`s address.
 * @param[in] *input_byte_length     Input data`s length.
 * @param[out] *input_byte_length    Length of input data that the codec actually uesd.
 *
 * @return
 * #AUDIO_CODEC_MANAGER_SUCCESS, process success. \n
 * #AUDIO_CODEC_MANAGER_ERROR,   process failed. \n
 *
 */
audio_codec_manager_status_t audio_codec_buffer_mode_process(audio_codec_manager_handle_t handle, uint8_t *input_buffer, uint32_t *input_byte_length);

/**
 * @brief     This function is to do codec process at buffer mode.
 *
 * @param[in] handle       Codec handel.
 *
 * @return    uint32_t  Length of processed data in codec at buffer mode. \n
 *
 */
uint32_t audio_codec_buffer_mode_get_output_data_length(audio_codec_manager_handle_t handle);

/**
 * @brief     This function is to get read address and length that can be read at once at buffer mode.
 *            For example, user can uses memcpy(*array, *address, *length) to get processed data from codec.
 *
 * @param[in] handle       Codec handel.
 * @param[in] **address    Read address.
 * @param[in] *length      Read length.
 *
 * @return
 * #AUDIO_CODEC_MANAGER_SUCCESS,  success. \n
 * #AUDIO_CODEC_MANAGER_ERROR,    failed. \n
 */
audio_codec_manager_status_t audio_codec_buffer_mode_get_read_information(audio_codec_manager_handle_t handle, uint8_t **address, uint32_t *length);

/**
 * @brief     This function is to do flush length of oldest processed data in codec at buffer.
 *
 * @param[in] handle       Codec handel.
 * @param[in] length       length of flushed data.
 *
 * @return
 * #AUDIO_CODEC_MANAGER_SUCCESS,  success. \n
 * #AUDIO_CODEC_MANAGER_ERROR,    failed. \n
 */
audio_codec_manager_status_t audio_codec_buffer_mode_read_done(audio_codec_manager_handle_t handle, uint32_t length);

/**
 * @brief     This function is to do flush all processed data in codec at buffer.
 *
 * @param[in] handle       Codec handel.
 *
 * @return
 * #AUDIO_CODEC_MANAGER_SUCCESS,  success. \n
 * #AUDIO_CODEC_MANAGER_ERROR,    failed. \n
 */
audio_codec_manager_status_t audio_codec_buffer_mode_clear_output_data(audio_codec_manager_handle_t handle);

/**
 * @brief     This function is to close codec.
 *
 * @param[in] handle       Codec handel.
 *
 * @return
 * #AUDIO_CODEC_MANAGER_SUCCESS,  success. \n
 * #AUDIO_CODEC_MANAGER_ERROR,    failed. \n
 */
audio_codec_manager_status_t audio_codec_close(audio_codec_manager_handle_t handle);

/**
 * @brief     This function is to parse the codec header to get some codec attributes, such channel number or sample rate etc.
 *
 * @param[in] handle       Codec handel.
 * @param[in] input_buffer The address of input data.
 * @param[in] input_byte_length  The length of input data.
 * @param[in] config Audio config information pass to codec.
 * @param[out] input_byte_length The consumed length of input data.
 * @param[out] config Audio config information pass to codec.
 * @return
 * #AUDIO_CODEC_MANAGER_SUCCESS,  success. \n
 * #AUDIO_CODEC_MANAGER_ERROR,    failed. \n
 * #AUDIO_CODEC_MANAGER_ERROR_LEN, the length of input data is not enough and refill data. \n
 */
audio_codec_manager_status_t audio_codec_parse_config(audio_codec_manager_handle_t handle, uint8_t *input_buffer, uint32_t *input_byte_length, audio_codec_manager_config_t *config);

/**
 * @brief     This function is to get the codec config pointer.
 *
 * @param[in] handle       Codec handel.
 * @return
 * #NULL, failed. \n
 * #others, the pointer of codec config, success. \n
 */
audio_codec_manager_config_t *audio_codec_get_codec_config(audio_codec_manager_handle_t handle);
#ifdef __cplusplus
}
#endif

#endif//__AUDIO_CODEC_MANAGER_H__