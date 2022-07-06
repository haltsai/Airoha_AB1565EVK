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

#ifndef __BSP_ADC_KEY_H__
#define __BSP_ADC_KEY_H__


/**
 * @addtogroup BSP
 * @{
 * @addtogroup ADC_KEY
 * @{
 * This section describes the programming interfaces of the adc key driver, which provides the driver to report basic key events to the upper layer.
 *
 * @section ADC_KEY_Terms_Chapter Terms and acronyms
 *
 *  @code
 *  void bsp_adc_key_callback((bsp_adc_key_channel_t channel, bsp_adc_key_event_t event, uint32_t event_data, uint32_t *user_data)
 *  {
 *      LOG_MSGID_I(adc_key, "channel=%d event=%d event_data=%d\r\n", 3, channel, event, event_data);
 *  }
 *
 *  void adckey_test(void)
 *  {
 *      bsp_adc_key_status_t key_status;
 *      bsp_adc_key_config_t adc_key_config;
 *      if (bsp_adc_key_init() != BSP_ADC_KEY_STATUS_OK) {
 *          LOG_MSGID_I(adc_key, "adc key init error\r\n", 0);
 *          return;
 *      }
 *      bsp_adc_key_register_callback(bsp_adc_key_callback, NULL);
 *      adc_key_config.adc_check_interval = 200;
 *      adc_key_config.adc_step = 226;
 *      adc_key_config.level_max = 17;
 *      adc_key_config.checking_reverse = true;
 *      adc_key_config.relative_value = false;
 *      key_status = bsp_adc_key_set_config(BSP_ADC_KEY_CHANNEL_0, &adc_key_config);
 *      if (BSP_ADC_KEY_STATUS_OK != key_status) {
 *          LOG_MSGID_I(adc_key, "adc key set config error, key_status=%d\r\n", 1, key_status);
 *          return;
 *      }
 *      adc_key_config.adc_check_interval = 220;
 *      adc_key_config.adc_step = 227;
 *      adc_key_config.checking_reverse = true;
 *      adc_key_config.relative_value = true;
 *      key_status = bsp_adc_key_set_config(BSP_ADC_KEY_CHANNEL_1, &adc_key_config);
 *      if (BSP_ADC_KEY_STATUS_OK != key_status) {
 *          LOG_MSGID_I(adc_key, "adc key set config error, key_status=%d\r\n", 1, key_status);
 *          return;
 *      }
 *      key_status = bsp_adc_key_enable(BSP_ADC_KEY_CHANNEL_0);
 *      if (BSP_ADC_KEY_STATUS_OK != key_status) {
 *          LOG_MSGID_I(adc_key, "adc key enable error, key_status=%d\r\n", 1, key_status);
 *          return;
 *      }
 *      key_status = bsp_adc_key_enable(BSP_ADC_KEY_CHANNEL_1);
 *      if (BSP_ADC_KEY_STATUS_OK != key_status) {
 *          LOG_MSGID_I(adc_key, "adc key enable error, key_status=%d\r\n", 1, key_status);
 *          return;
 *      }
 *  }
 *
 *  @endcode
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include "hal_gpt.h"
#include "hal_gpio.h"
#include "hal_adc.h"

/** @defgroup adc_key_struct Struct
  * @{
  */

/** @defgroup adc_key_enum Enums
  * @{
  */
typedef enum {
    BSP_ADC_KEY_STATUS_OK              = 0,
    BSP_ADC_KEY_STATUS_ERROR           = -1,
    BSP_ADC_KEY_STATUS_INVALID_CHANNEL = -2,
    BSP_ADC_KEY_STATUS_NOT_INIT        = -3,
} bsp_adc_key_status_t;

/** @brief adc channel */
typedef enum {
    BSP_ADC_KEY_CHANNEL_0 = 0,                        /**< ADC key channel 0. */
    BSP_ADC_KEY_CHANNEL_1 = 1,                        /**< ADC key channel 1. */
    BSP_ADC_KEY_CHANNEL_2 = 2,                        /**< ADC key channel 2. */
    BSP_ADC_KEY_CHANNEL_MAX = 3                       /**< The total number of ADC key channels (invalid ADC key channel).*/
} bsp_adc_key_channel_t;

/** @brief This enum defines key event type. */
typedef enum {
    BSP_ADC_KEY_VAL_INIT      = 0,        /**< The first event*/
    BSP_ADC_KEY_VAL_UP        = 1,        /**< ADC value increase */
    BSP_ADC_KEY_VAL_DOWN      = 2,        /**< ADC value decrease */
    BSP_ADC_KEY_VAL_ABS       = 3,        /**< ADC value is abaolute value */
} bsp_adc_key_event_t;
/**
* @}
*/

/** @brief  This defines the callback function prototype.
 *          User should register a callback function when using adc key, this function will be called in external interrupt
 *          service routine after a key is pressed or released.
 *  @param [in] event is the key event for the current key status, user can get the key status from this parameter, please
 *              refer to #bsp_adc_key_event_t for details about the event type.
 *  @param [in] channel is used to identify a specific key.
 *  @param [in] event_data is the current level of the adc.
 *  @param [in] user_data is a user-defined parameter provided by #bsp_adc_key_register_callback().
 *  @sa  #bsp_adc_key_register_callback()
 */
typedef void (*bsp_adc_key_callback_t)(bsp_adc_key_channel_t channel, bsp_adc_key_event_t event, uint32_t event_data, uint32_t *user_data);

typedef struct {
    uint16_t adc_check_interval; /**< internal timer period */
    uint8_t adc_step;            /**< The adc step used to distinguish adc level */
    uint8_t level_max;           /**< To decide the maxium of event_data */
    bool checking_reverse;       /**< This used to adjust the event_data change direction */
    bool relative_value;         /**< The event_data is relative value or absolute value */
} bsp_adc_key_config_t;

/**
* @}
*/

/**
 * @brief ADC key initialization.
 *
 * @return #BSP_ADC_KEY_STATUS_ERROR, if initialize ADC failed.
 *         #BSP_ADC_KEY_STATUS_OK, if initialize ADC key successfully.
 */
bsp_adc_key_status_t bsp_adc_key_init(void);

/**
 * @brief ADC key de-initialization.
 *
 * @return #BSP_ADC_KEY_STATUS_ERROR, if de-initialize ADC failed.
 *         #BSP_ADC_KEY_STATUS_OK, if de-initialize ADC key successfully.
 */
bsp_adc_key_status_t bsp_adc_key_deinit(void);

/**
 * @brief This is used to configure the ADC key
 *
 * @param channel is the ADC channel.
 * @param config  is the configurations.
 * @return #BSP_ADC_KEY_STATUS_INVALID_CHANNEL, if channel is out of range.
 *         #BSP_ADC_KEY_STATUS_NOT_INIT, if the ADC key is not initialized.
 *         #BSP_ADC_KEY_STATUS_OK, if configure channel successfully.
 */
bsp_adc_key_status_t bsp_adc_key_set_config(bsp_adc_key_channel_t channel, bsp_adc_key_config_t *config);

/**
 * @brief This is used to register the callback function of ADC key to receive event.
 *
 * @param callback is the callback function.
 * @param usr_data is the user-defined parameter.
 * @return #BSP_ADC_KEY_STATUS_ERROR, if callback is NULL.
 *         #BSP_ADC_KEY_STATUS_OK, if get ADC value successfully.
 */
bsp_adc_key_status_t bsp_adc_key_register_callback(bsp_adc_key_callback_t callback, void *usr_data);

/**
 * @brief This is used to enable ADC key event detection
 *
 * @param channel is the ADC channel.
 * @return #BSP_ADC_KEY_STATUS_INVALID_CHANNEL, if channel is out of range.
 *         #BSP_ADC_KEY_STATUS_NOT_INIT, if the ADC key is not initialized.
 *         #BSP_ADC_KEY_STATUS_ERROR, if start timer failed.
 *         #BSP_ADC_KEY_STATUS_OK, if enable channel successfully.
 */
bsp_adc_key_status_t bsp_adc_key_enable(bsp_adc_key_channel_t channel);

/**
 * @brief This is used to disable ADC key event detection
 *
 * @param channel is the ADC channel.
 * @return #BSP_ADC_KEY_STATUS_INVALID_CHANNEL, if channel is out of range.
 *         #BSP_ADC_KEY_STATUS_NOT_INIT, if the ADC key is not initialized.
 *         #BSP_ADC_KEY_STATUS_ERROR, if stop timer failed.
 *         #BSP_ADC_KEY_STATUS_OK, if disable channel successfully.
 */
bsp_adc_key_status_t bsp_adc_key_disable(bsp_adc_key_channel_t channel);

/**
 * @brief This is used to get the ADC sample value.
 *
 * @param channel is the ADC channel.
 * @param data is the value sample from channel.
 * @return #BSP_ADC_KEY_STATUS_INVALID_CHANNEL, if channel is out of range.
 *         #BSP_ADC_KEY_STATUS_NOT_INIT, if the ADC key is not initialized.
 *         #BSP_ADC_KEY_STATUS_OK, if get ADC value successfully.
 */
bsp_adc_key_status_t bsp_adc_key_get_data(bsp_adc_key_channel_t channel, uint32_t *data);

/**
 * @brief This is used to get current ADC level.
 *
 * @param channel is the ADC channel.
 * @param level is the current ADC level.
 * @return #BSP_ADC_KEY_STATUS_INVALID_CHANNEL, if channel is out of range.
 *         #BSP_ADC_KEY_STATUS_NOT_INIT, if the ADC key is not initialized.
 *         #BSP_ADC_KEY_STATUS_OK, if get ADC level successfully.
 */
bsp_adc_key_status_t bsp_adc_key_get_level(bsp_adc_key_channel_t channel, uint32_t *level);

/**
 * @brief This is used to get the total ADC key channel
 *
 * @param channel_max is the total ADC key channel that have been configured.
 * @return #BSP_ADC_KEY_STATUS_NOT_INIT, if the ADC key is not initialized.
 *         #BSP_ADC_KEY_STATUS_ERROR, if channel_max is NULL.
 *         #BSP_ADC_KEY_STATUS_OK, if get the total ADC key channel successfully.
 */
bsp_adc_key_status_t bsp_adc_key_get_channel(uint32_t *channel_max);

#ifdef __cplusplus
}
#endif
/**
* @}
* @}
*/
#endif /* __BSP_ADC_KEY_H__ */

