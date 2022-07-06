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


#ifndef __BSP_PSENSOR_KEY_H__
#define __BSP_PSENSOR_KEY_H__

/**
 * @addtogroup BSP
 * @{
 * @addtogroup ProximitySensor_key
 * @{
 * This section describes the programming interfaces of the ProximitySensor key driver, which provides the driver to report basic key events to the upper layer.
 *
 * @section ProximitySensor_key_Features_Chapter Supported features
 *
 * The ProximitySensor key can report 2 events to upper layer, please refer to #bsp_psensor_key_event_t.
 *
 * @section ProximitySensor_key_High_Level_Driver_Usage_Chapter How to use the ProximitySensor key
 * User only needs to call #bsp_psenosr_key_init to initialize the ProximitySensor key.
 *
 * @code
 *
 *   static void bsp_psensor_callback(uint8_t key_data, bsp_psensor_key_event_t event, void *user_data)
 *   {
 *      switch(event) {
 *      case BSP_PSENSOR_KEY_FAR:
 *          // Perform the action according to the specific event type.
 *          break;
 *      case BSP_PSENSOR_KEY_NEAR:
 *          // Perform the action according to the specific event type.
 *          break;
 *      default:
 *          break;
 *      }

 *   }
 *
 *   bsp_psensor_key_config_t config;
 *
 *   config.callback.callback    = bsp_psensor_callback;
 *   config.callback.user_data   = NULL;
 *
 *   config.i2c.port             = HAL_I2C_MASTER_0;
 *   config.i2c.conifg.frequency = HAL_I2C_FREQUENCY_400K;
 *
 *   config.key_data = DEVICE_KEY_SPACE;
 *
 *   bsp_psensor_key_init(&config);
 *
 * @endcode
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "hal.h"

/** @brief This enum defines key event status. */
typedef enum {
    BSP_PSENSOR_KEY_FAR   = 0,
    BSP_PSENSOR_KEY_NEAR  = 1
} bsp_psensor_key_event_t;


/** @brief  This defines the callback function prototype.
 *          User should register a callback function when using proximity sensor key, this function will be called in external interrupt
 *          service routine after a proximity sensor key is tapped.
 *  @param [in] key_data is used to identify a specific key.
 *  @param [in] event is the key event for the current key status, user can get the key status from this parameter, please
 *              refer to #bsp_psensor_key_event_t for details about the event type.
 *  @param [in] user_data is a user-defined parameter provided by #bsp_psenosr_key_init().
 *  @sa  #bsp_psenosr_key_init()
 */
typedef void (*bsp_psensor_key_callback_t)(uint8_t key_data, bsp_psensor_key_event_t event, void *user_data);

/** @brief This structure defines the callback type. */
typedef struct {
    bsp_psensor_key_callback_t callback;  /**< callback will be called when a key event must be reported to user.*/
    void *user_data;                       /**< a parameter of the callback function. */
} psensor_callback_t;

/** @brief This structure defines the i2c configuration for Gsensor. */
typedef struct {
    hal_i2c_port_t      port;       /*Used i2c port number.*/
    hal_i2c_config_t    conifg;     /*Used i2c port configuration.*/
} psensor_i2c_config_t;

/** @brief This structure defines the configuration for proximity sensor key. */
typedef struct {
    uint8_t                  key_data;  /**< keydata define by user. It passes to callback function
                                                    #bsp_psensor_key_callback_t as the first parameter.*/
    psensor_i2c_config_t     i2c;       /**< the I2C driver configuration.*/
    psensor_callback_t      callback;  /**< the callback structure.*/
} bsp_psensor_key_config_t;


/**
 * @brief This function is used to initialize proximity sensor key.
 *
 * @param [in] gsensor_config  is Gsensor key configuration.
 * @return true means sensor is initialized success, false means sensor is initialized failed.
 */
bool bsp_psensor_key_init(bsp_psensor_key_config_t *psensor_config);

/**
 * @brief This function is used to deinitialize proximity sensor key.
 *
 * @return none
 */
void bsp_psensor_key_deinit(void);

#ifdef __cplusplus
}
#endif

#endif //_BSP_PSENSOR_KEY_H__
