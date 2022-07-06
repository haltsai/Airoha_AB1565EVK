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
#ifndef __AUDIO_TRANSMITTER_CONTROL_H__
#define __AUDIO_TRANSMITTER_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "hal_audio_message_struct.h"
#include "audio_transmitter_control_port.h"
#include "FreeRTOS.h"


#define TRANSMITTER_LOG_E(fmt,arg...)   LOG_MSGID_E(audio_transmitter, "[audio_transmitter]"fmt,##arg)
#define TRANSMITTER_LOG_W(fmt,arg...)   LOG_MSGID_W(audio_transmitter, "[audio_transmitter]"fmt,##arg)
#define TRANSMITTER_LOG_I(fmt,arg...)   LOG_MSGID_I(audio_transmitter, "[audio_transmitter]"fmt,##arg)

/**
 * @addtogroup Audio
 * @{
 * @addtogroup audio_transmitter
 * @{
 *
 * The audio_transmitter is used to control some specific applications.
 *
 * @section audio_transmitter_api_usage How to use this module
 *
 * - An example on how to use the audio_transmitter APIs.\n
 *  - 1.  Call #audio_transmitter_init() to init audio_transmitter and get audio_transmitter id.
 *  - 2.  Call #audio_transmitter_start() to start this senario.
 *        When event AUDIO_TRANSMITTER_EVENT_START_SUCCESS is received, the scenario has successfully started.
 *  - 3.  If this scenario can set config param, call #audio_transmitter_set_runtime_config() to set config.
 *  - 4.  If this scenario can get config param, call #audio_transmitter_get_runtime_config() to get config.
 *  - 5.  If this scenario needs receive data from dsp side, \n
 *        call #audio_transmitter_get_read_information() to get data address and data length,
 *        call #audio_transmitter_read_done() to flush data.
 *  - 6.  If this scenario needs send data to dsp side, \n
 *        call #audio_transmitter_get_write_information() to get the address where the data is placed and maximum length that can be placed.
 *        call #audio_transmitter_write_done() to send data.
 *  - 7.  Call #audio_transmitter_stop() to stop this senario.
 *  - 8.  Call #audio_transmitter_deinit() to deinit this senario.
 *    - Sample code:
 *     @code
 *
 *       static void test_audio_transmitter_event_callback(audio_transmitter_event_t event,void *data, void *user_data)
 *       {
 *           uint32_t cb_data = 0;
 *           uint8_t *address;
 *           uint32_t length;
 *           transmitter_user_t *user = (transmitter_user_t *)user_data;
 *
 *           switch (event) {
 *               case AUDIO_TRANSMITTER_EVENT_DATA_NOTIFICATION:
 *                   printf("receive data notification.");
 *                   break;
 *               case AUDIO_TRANSMITTER_EVENT_DATA_DIRECT:
 *                   if(data != NULL){
 *                       cb_data = *((uint32_t*)data);
 *                   }
 *                   printf("receive direct data %d.", cb_data);
 *                   break;
 *               case AUDIO_TRANSMITTER_EVENT_START_SUCCESS:
 *                   printf("audio transmitter start success.");
 *                   break;
 *               case AUDIO_TRANSMITTER_EVENT_START_FAIL:
 *                   printf("audio transmitter start fail.");
 *                   break;
 *               case AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS:
 *                   printf("audio transmitter stop success.");
 *                   break;
 *               default:
 *                   printf("audio transmitter event undefined.");
 *                   break;
 *            }
 *        }
 *
 *        void audio_transmitter_demo()
 *        {
 *             audio_transmitter_id_t id;
 *             audio_transmitter_status_t status;
 *             audio_transmitter_config_t config;
 *
 *             config.scenario_type = AUDIO_TRANSMITTER_TEST;
 *             config.scenario_sub_id = AUDIO_TRANSMITTER_TEST_AUDIO_LOOPBACK;
 *             config.msg_handler = test_audio_transmitter_event_callback;
 *             config.user_data = NULL;
 *             config.scenario_config.audio_transmitter_test_config.audio_loopback_test_config.audio_device = HAL_AUDIO_DEVICE_MAIN_MIC_L;
 *             config.scenario_config.audio_transmitter_test_config.audio_loopback_test_config.audio_interface = HAL_AUDIO_INTERFACE_1;
 *             id = audio_transmitter_init(&config);
 *             if(id < 0) {
 *                 printf("audio transmitter init failed.");
 *                 return;
 *             }
 *
 *             status = audio_transmitter_start(id);
 *             if (status != AUDIO_TRANSMITTER_STATUS_SUCCESS) {
 *                 printf("audio transmitter can not start this id.");
 *                 return;
 *             }
 *             vTaskDelay(5000 / portTICK_RATE_MS);
 *
 *             status = audio_transmitter_stop(id);
 *             if (status != AUDIO_TRANSMITTER_STATUS_SUCCESS) {
 *                 printf("audio transmitter can not stop this id.");
 *                 return;
 *             }
 *             vTaskDelay(5000 / portTICK_RATE_MS);
 *
 *             status = audio_transmitter_deinit(id);
 *             if (status != AUDIO_TRANSMITTER_STATUS_SUCCESS) {
 *                 printf("audio transmitter can not deinit this id.");
 *                 return;
 *             }
 *
 *      @endcode
 */

/** @defgroup audio_transmitter_enum Enum
  * @{
  */

/** @brief This enum defines the audio transmitter return types */
typedef enum {
    AUDIO_TRANSMITTER_STATUS_FAIL = -1,    /**< The audio_transmitter func return fail.   */
    AUDIO_TRANSMITTER_STATUS_SUCCESS = 0   /**< The audio_transmitter func return success.   */
} audio_transmitter_status_t;

/** @brief This enum defines the audio transmitter state */
typedef enum {
    AUDIO_TRANSMITTER_STATE_CLOSE   = 0,    /**< The audio_transmitter state is closed. Not init yet  */
    AUDIO_TRANSMITTER_STATE_IDLE    = 1,    /**< The audio_transmitter state is idle, or is stopped.   */
    AUDIO_TRANSMITTER_STATE_STARTING = 2,   /**< The audio_transmitter state is going to start, has not started yet.   */
    AUDIO_TRANSMITTER_STATE_STARTED  = 3,   /**< The audio_transmitter state is started, finish start.   */
    AUDIO_TRANSMITTER_STATE_STOPING = 4,    /**< The audio_transmitter state is going to stop, is not stopped yet.   */
    AUDIO_TRANSMITTER_STATE_MAX
} audio_transmitter_state_t;

/** @brief This enum defines the audio transmitter event */
typedef enum {
    AUDIO_TRANSMITTER_EVENT_START_SUCCESS,                /**< Start success.   */
    AUDIO_TRANSMITTER_EVENT_START_FAIL,                   /**< Start fail.   */
    AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS,                 /**< Stop success.   */
    AUDIO_TRANSMITTER_EVENT_DATA_NOTIFICATION,            /**< Receive data notification from dsp side.   */
    AUDIO_TRANSMITTER_EVENT_DATA_DIRECT,                  /**< Receive direct 32bit data from dsp side.   */
    AUDIO_TRANSMITTER_EVENT_SET_RUNTIME_CONFIG_SUCCESS,   /**< Set runtime config success.   */
    AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_HFP,          /**< Start fail, reject by HFP.   */
    AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_HFP,               /**< Be stopped by HFP.   */
    AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_RECORDER,     /**< Start fail, reject by recorder.   */
    AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_RECORDER,          /**< Be stopped by recorder.   */
    AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_A2DP,         /**< Start fail, reject by A2DP.   */
    AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_A2DP,              /**< Be stopped by A2DP.   */
    AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_LINE_OUT,          /**< Be stopped by line-out.   */
    AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_USB_OUT,           /**< Be stopped by usb-out.   */
    AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_LINE_IN,           /**< Be stopped by line-in.   */
    AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_USB_IN,            /**< Be stopped by usb-in.   */
    AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_LE_CALL,      /**< Start fail, reject by LE_CALL.   */
    AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_LE_CALL,           /**< Be stopped by LE_CALL.   */
} audio_transmitter_event_t;

/**
  * @}
  */


/** @defgroup audio_transmitter_typedef Typedef
  * @{
  */

/** @brief  This defines the audio transmitter callback function prototype. \n
 *          Register a callback function to receive audio transmitter event. \n
 *          For more details about the event, please refer to #audio_transmitter_event_t.
 *  @param[in]  event       The value defined in #audio_transmitter_event_t. This parameter is given by the audio transmitter to notify the user about the event.
 *  @param[in]  data        The point to data coming with event.
 *  @param[in]  user_data   A user defined parameter.
 */
typedef void (*audio_transmitter_msg_handler_t)(audio_transmitter_event_t event, void *data, void *user_data);

/** @brief This defines the audio transmitter id*/
typedef int8_t audio_transmitter_id_t;          /**< audio_transmitter id, returned when init. User get this id to use audio_transmitter. */

/**
 * @}
 */


/** @defgroup audio_transmitter_struct Struct
 *  @{
 */

/** @brief This struct defines the config informations for init audio transmitter*/

typedef struct {
    audio_transmitter_scenario_type_t scenario_type;     /**< Audio_transmitter scenario type. */
    uint8_t scenario_sub_id;                             /**< Audio_transmitter scenario sub type. */
    scenario_config_t scenario_config;                   /**< Audio_transmitter init scenario param. */
    audio_transmitter_msg_handler_t msg_handler;         /**< User hang func, to receive audio_transmitter event. */
    void *user_data;                                     /**< Point for user to use in audio_transmitter. */
} audio_transmitter_config_t;

/**
 * @}
 */


/**
 * @brief     This function is to init audio transmitter.
 *
 * @param[in] *config     Audio config information pass to audio transmitter, please refer to #audio_transmitter_config_t.
 * @return    id          Audio_transmitter id will >= 0, if init fail id = -1.
 */
audio_transmitter_id_t audio_transmitter_init(audio_transmitter_config_t *config);

/**
 * @brief     This function is to start audio transmitter.
 *
 * @param[in] id     Audio transmitter id.
 * @return
 * #AUDIO_TRANSMITTER_STATUS_SUCCESS,  Able to start. When user gets the AUDIO_TRANSMITTER_EVENT_START_SUCCESS event, it has successfully started. \n
 * #AUDIO_TRANSMITTER_STATUS_FAIL,    Failed. User start this scenario failed.
 */
audio_transmitter_status_t audio_transmitter_start(audio_transmitter_id_t id);

/**
 * @brief     This function is to stop audio transmitter.
 *
 * @param[in] id     Audio transmitter id.
 * @return
 * #AUDIO_TRANSMITTER_STATUS_SUCCESS,  Able to stop. When user gets the AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS event, it has successfully stopped. It is an asynchronous event. \n
 * #AUDIO_TRANSMITTER_STATUS_FAIL,    Failed. User stop this scenario failed.
 */
audio_transmitter_status_t audio_transmitter_stop(audio_transmitter_id_t id);

/**
 * @brief     This function is to deinit audio transmitter.
 *
 * @param[in] id     Audio transmitter id.
 * @return
 * #AUDIO_TRANSMITTER_STATUS_SUCCESS,  Success.\n
 * #AUDIO_TRANSMITTER_STATUS_FAIL,    Failed.
 */
audio_transmitter_status_t audio_transmitter_deinit(audio_transmitter_id_t id);

/**
 * @brief     This function is to get total available data size that can be read from audio transmitter.
 *
 * @param[in] id           Audio transmitter id.
 *
 * @return    uint32_t  Length of otal available data size. \n
 *
 */
uint32_t audio_transmitter_get_available_data_size(audio_transmitter_id_t id);

/**
 * @brief     This function is to get total available space size that can be written to audio transmitter.
 *
 * @param[in] id           Audio transmitter id.
 *
 * @return    uint32_t  Length of total available space size. \n
 *
 */
uint32_t audio_transmitter_get_available_data_space(audio_transmitter_id_t id);

/**
 * @brief     This function is to get read address and length that can be read at once from audio transmitter.
 *            For example, user can uses memcpy(*array, *address, *length) to get processed data from audio transmitter.
 *
 * @param[in] id           Audio transmitter id.
 * @param[out] **address   Get data address.
 * @param[out] *length     Get data length.
 *
 * @return
 * #AUDIO_TRANSMITTER_STATUS_SUCCESS,  Success. \n
 * #AUDIO_TRANSMITTER_STATUS_FAIL,    Failed.
 */
audio_transmitter_status_t audio_transmitter_get_read_information(audio_transmitter_id_t id, uint8_t **address, uint32_t *length);

/**
 * @brief     This function is to flush one block data in audio transmitter.
 *            User should use this API(the length must same with the length get by audio_transmitter_get_read_information()) when finish process the current block.
 *            When user call audio_transmitter_read_done(id, length), in these scenarios, it means user finish process current block and switch to next block.
 *            if the length less than expected length, API will do nothing and return FAIL.
 *
 * @param[in] id           Audio transmitter id
 * @param[in] length       Length of data to flush.
 *
 * @return
 * #AUDIO_TRANSMITTER_STATUS_SUCCESS,  Success. \n
 * #AUDIO_TRANSMITTER_STATUS_FAIL,    Failed.
 */
audio_transmitter_status_t audio_transmitter_read_done(audio_transmitter_id_t id, uint32_t length);

/**
 * @brief     This function is to get write address and maximum length that can be written to audio transmitter at once.
 *            For example, user can uses memcpy(*address, *array, *length) to get processed data to audio transmitter.
 *
 * @param[in] id           Audio transmitter id.
 * @param[out] **address   Get write address.
 * @param[out] *length     Get maximum length of data that can be put in place.
 *
 * @return
 * #AUDIO_TRANSMITTER_STATUS_SUCCESS,  Success. \n
 * #AUDIO_TRANSMITTER_STATUS_FAIL,    Failed.
 */
audio_transmitter_status_t audio_transmitter_get_write_information(audio_transmitter_id_t id, uint8_t **address, uint32_t *length);

/**
 * @brief     This function is to send one block data to audio transmitter.
 *            User should use this API(the length must less than or equal to with the length get by audio_transmitter_get_write_information()) when finish process the current block.
 *            When user call audio_transmitter_write_done(id, length), in these scenarios, it means user finish process current block and switch to next block.
 *            if the length more than expected length, API will do nothing and return FAIL.
 *
 * @param[in] id           Audio transmitter id
 * @param[in] length       Length of data written in one block.
 *
 * @return
 * #AUDIO_TRANSMITTER_STATUS_SUCCESS,  Success. \n
 * #AUDIO_TRANSMITTER_STATUS_FAIL,    Failed.
 */
audio_transmitter_status_t audio_transmitter_write_done(audio_transmitter_id_t id, uint32_t length);

/**
 * @brief     This function is set config when transmitter is started.
 *
 * @param[in] id                 Audio transmitter id
 * @param[in] config_type        Type of config. Each scenario has its own type list.
 * @param[in] *runtime_config    Parameter to config.
 *
 * @return
 * #AUDIO_TRANSMITTER_STATUS_SUCCESS,  Success. \n
 * #AUDIO_TRANSMITTER_STATUS_FAIL,    Failed.
 */
audio_transmitter_status_t audio_transmitter_set_runtime_config(audio_transmitter_id_t id, audio_transmitter_runtime_config_type_t config_type, audio_transmitter_runtime_config_t *runtime_config);

/**
 * @brief     This function is get config parameter when transmitter is started.
 *
 * @param[in] id                 Audio transmitter id
 * @param[in] config_type        Type of config. Each scenario has its own type list.
 * @param[out] *runtime_config   Param to get.
 *
 * @return
 * #AUDIO_TRANSMITTER_STATUS_SUCCESS,  Success. \n
 * #AUDIO_TRANSMITTER_STATUS_FAIL,    Failed.
 */
audio_transmitter_status_t audio_transmitter_get_runtime_config(audio_transmitter_id_t id, audio_transmitter_runtime_config_type_t config_type, audio_transmitter_runtime_config_t *runtime_config);


#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/

#endif/*__AUDIO_TRANSMITTER_RECEIVE_CONTROL_H__*/
