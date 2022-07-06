/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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

/**
 * File: audio_source_control.h
 *
 * Description: This file use for audio source control (request/release resource) for dual-chip case.
 *  Request and release audio source from the audio service.
 */

#ifndef __AUDIO_SOURCE_CONTROL_H__
#define __AUDIO_SOURCE_CONTROL_H__

#include "audio_src_srv_resource_manager_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 *@addtogroup Audio source control
 *@{
 * This section introduces the interfaces including the structures, the enumurations, and the details on how to use the interfaces.
 *
 *@}
 */

/**
  * @addtogroup Audio source control
  * @{
  */

/**
 * @brief The Audio source control event type
 *
 */
typedef enum {
    AUDIO_SOURCE_CONTROL_EVENT_RES_CTRL                 = 0x00,
    AUDIO_SOURCE_CONTROL_EVENT_TRANSMITTER              = 0x01,
    AUDIO_SOURCE_CONTROL_EVENT_VENDER_SE                = 0x02,
    AUDIO_SOURCE_CONTROL_EVENT_CMD_FAIL                 = 0x03,
} audio_source_control_event_type;

/**
 * @brief The audio source control function return value
 *
 */
typedef enum {
    AUDIO_SOURCE_CONTROL_RESULT_OK                      = 0x00,     /**< Execute succeed. */
    AUDIO_SOURCE_CONTROL_RESULT_WRONG_REQUEST_TYPE      = 0x01,     /**< Execute failed because of the invalid request type. */
    AUDIO_SOURCE_CONTROL_RESULT_NULL_HANDLE             = 0x02,     /**< Execute failed because handle is NULL. */
    AUDIO_SOURCE_CONTROL_RESULT_HANDLE_NOT_EXIST        = 0x03,     /**< Execute failed because the handle not exist. */
    AUDIO_SOURCE_CONTROL_RESULT_RESOURCE_NOT_EXIST      = 0x05,     /**< Execute failed because the source does not exist. */
} audio_source_control_result_t;

typedef enum {
    AUDIO_SOURCE_CONTROL_CMD_REQUEST_RES                = 0x00,
    AUDIO_SOURCE_CONTROL_CMD_RELEASE_RES                = 0x01,
    AUDIO_SOURCE_CONTROL_CMD_ADD_WAITTING_LIST          = 0x02,
    AUDIO_SOURCE_CONTROL_CMD_DEL_WAITTING_LIST          = 0x03,
    AUDIO_SOURCE_CONTROL_CMD_START_TRANSMITTER          = 0x04,
    AUDIO_SOURCE_CONTROL_CMD_STOP_TRANSMITTER           = 0x05,
    AUDIO_SOURCE_CONTROL_CMD_START_SIDETONE             = 0x06,
    AUDIO_SOURCE_CONTROL_CMD_STOP_SIDETONE              = 0x07,
} audio_source_control_cmd_t;

typedef enum {
    AUDIO_SOURCE_CONTROL_CMD_DEST_LOCAL                 = 0x00,
    AUDIO_SOURCE_CONTROL_CMD_DEST_REMOTE                = 0x01,
} audio_source_control_cmd_dest_t;

typedef enum {
    AUDIO_SOURCE_CONTROL_USR_AMA                        = 0x00,
    AUDIO_SOURCE_CONTROL_USR_GSOUND                     = 0x01,
    AUDIO_SOURCE_CONTROL_USR_LINE_IN                    = 0x02,
    AUDIO_SOURCE_CONTROL_USR_LINE_OUT                   = 0x03,
    AUDIO_SOURCE_CONTROL_USR_USB_IN                     = 0x04,
    AUDIO_SOURCE_CONTROL_USR_USB_OUT                    = 0x05,
    AUDIO_SOURCE_CONTROL_USR_ALL                        = 0xFE
} audio_source_control_usr_type_t;

/**
 * @brief The definition of the audio source request execute result.
 *
 */
typedef struct {
    audio_src_srv_resource_type_t res_type;
    unsigned char req_priority;
    audio_source_control_usr_type_t usr;
    uint8_t* usr_name;
    void* usr_data;
    void (*request_notify)(audio_source_control_event_type type, uint32_t sub_event, void* usr_data); /**< Audio source request execute result notify handler. */
} audio_source_control_cfg_t;

/**
 * @brief Init the audio source control module.
 *
 */
void audio_source_control_init();

/**
 * @brief De-init the audio source control module.
 *
 */
void audio_source_control_deinit();

/**
 * @brief Register the module to the audio source control module.
 *
 * @param[in] cfg the audio resource configuration
 * @return return the audio source control handle to the module.
 */
void *audio_source_control_register(audio_source_control_cfg_t* cfg);

/**
 * @brief Un-register the module, the handle cannot be used any more after un-registered.
 *
 * @param[in] handle the handle that returned from the #audio_source_control_register function.
 */
void audio_source_control_unregister(void *handle);

/**
 * @brief Send the result by the sub-module which customized the audio source control flow.
 *
 * @param[in] handle_id the ID that registered in the function #audio_source_control_register
 * @param[in] result the result which need send to user that request the audio source.
 * @return return the error code to indicate function result.
 */
audio_source_control_result_t audio_source_control_cmd(void *handle,
                    audio_source_control_cmd_t cmd,
                    audio_source_control_cmd_dest_t cmd_dest);

/**
 * @brief The is for UT testing
 *
 */
#define AUDIO_SOURCE_CONTROL_TEST 0

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AUDIO_SOURCE_CONTROL_H__ */


