/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
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

#ifndef _BSP_GSENSOR_KEY_H_
#define _BSP_GSENSOR_KEY_H_

/**
 * @addtogroup BSP
 * @{
 * @addtogroup Gsensor_key
 * @{
 * This section describes the programming interfaces of the Gsensor key driver, Which provides the driver to report basic key events to the upper layer.
 *
 * @section Gsensor_key_Terms_Chapter Terms and acronyms
 *
 * |Terms            |Details                                                                 |
 * |-----------------|------------------------------------------------------------------------|
 * |\b Gsensor key   | the key is implemented by Gravity sensor.|
 *
 * @section Gsensor_key_Features_Chapter Supported features
 *
 * The Gsensor key can report 2 events to upper layer, please refer to #bsp_gsensor_key_event_t.
 *
 * @section Gsensor_key_High_Level_Driver_Usage_Chapter How to use the Gsensor key
 * User only needs to call #bsp_gsenosr_key_init to initialize the Gsensor key.
 *
 * @code
 *
 *   static void bsp_gsensor_callback(uint8_t key_data, bsp_gsensor_key_event_t event, void *user_data)
 *   {
 *      switch(event) {
 *      case BSP_GSENSOR_KEY_SINGLE_TAP:
 *          // Perform the action according to the specific event type.
 *          break;
 *      case BSP_GSENSOR_KEY_DOUBLE_TAP:
 *          // Perform the action according to the specific event type.
 *          break;
 *      default:
 *          break;
 *      }

 *   }
 *
 *   bsp_gsensor_key_config_t config;
 *
 *   hal_gpt_status_t ret;
 *
 *   config.callback.callback    = bsp_gsensor_callback;
 *   config.callback.user_data   = NULL;
 *
 *   config.i2c.port             = HAL_I2C_MASTER_0;
 *   config.i2c.conifg.frequency = HAL_I2C_FREQUENCY_400K;
 *
 *   config.key_data = DEVICE_KEY_A;
 *
 *   bsp_gsenosr_key_init(&config);
 *
 * @endcode
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "hal.h"

/** @defgroup Gsensor_key_enum Enums
  * @{
  */
/** @brief This enum defines key event status. */
typedef enum {
    BSP_GSENSOR_KEY_SINGLE_TAP = 1,
    BSP_GSENSOR_KEY_DOUBLE_TAP = 2
} bsp_gsensor_key_event_t;
/**
* @}
*/

/** @defgroup Gsensor_key_typedef Typedef
  * @{
  */

/** @brief  This defines the callback function prototype.
 *          User should register a callback function when using Gsensor key, this function will be called in external interrupt
 *          service routine after a Gsensor key is tapped.
 *  @param [in] key_data is used to identify a specific key.
 *  @param [in] event is the key event for the current key status, user can get the key status from this parameter, please
 *              refer to #bsp_gsensor_key_event_t for details about the event type.
 *  @param [in] user_data is a user-defined parameter provided by #bsp_gsenosr_key_init().
 *  @sa  #bsp_gsenosr_key_init()
 */
typedef void (*bsp_gsensor_key_callback_t)(uint8_t key_data, bsp_gsensor_key_event_t event, void *user_data);
/**
* @}
*/

/** @defgroup Gsensor_key_struct Struct
  * @{
  */
/** @brief This structure defines the callback type. */
typedef struct {
    bsp_gsensor_key_callback_t callback;  /**< callback will be called when a key event must be reported to user.*/
    void *user_data;                      /**< a parameter of the callback function. */
} bsp_gsensor_callback_t;

/** @brief This structure defines the i2c configuration for Gsensor. */
typedef struct {
    hal_i2c_port_t      port;       /**< Used i2c port number.*/
    hal_i2c_config_t    conifg;     /**< Used i2c port configuration.*/
} bsp_gensor_i2c_config_t;

/** @brief This structure defines the configuration for Gsensor key. */
typedef struct {
    uint8_t                     key_data;         /**< keydata define by user. It passes to callback function
                                                    #bsp_gsensor_key_callback_t as the first parameter.*/
    bsp_gensor_i2c_config_t     i2c;              /**< the I2C driver configuration.*/
    bsp_gsensor_callback_t      callback;         /**< the callback structure.*/
} bsp_gsensor_key_config_t;

/**
* @}
*/

/**
 * @brief This function is used to initialize Gsensor key.
 *
 * @param gsensor_config is Gsensor key configuration.
 * @return none
 */
void bsp_gsenosr_key_init(bsp_gsensor_key_config_t *gsensor_config);

#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/
#endif /*_BSP_GSENSOR_KEY_H_*/

