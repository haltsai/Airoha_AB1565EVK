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

#ifndef _BSP_EINT_KEY_H_
#define _BSP_EINT_KEY_H_


/**
 * @addtogroup BSP
 * @{
 * @addtogroup EINT_KEY
 * @{
 * This section describes the programming interfaces of the eint key driver, which provides the driver to report basic key events to the upper layer.
 *
 * @section EINT_KEY_Terms_Chapter Terms and acronyms
 *
 * |Terms            |Details                                                                 |
 * |-----------------|------------------------------------------------------------------------|
 * |\b EINT KEY      | the key is implemented by external interrupt|
 *
 * @section EINT_KEY_Features_Chapter Supported features
 *
 * The eint key can report 4 events to upper layer, please refer to #bsp_eint_key_event_t, and event timing is adjustable.
 *
 * @section EINT_KEY_High_Level_Driver_Usage_Chapter How to use the eint key
 *  The steps are shown below:
 *   - Step 1: Call #bsp_eint_key_init() to initialize the eint key module.
 *   - Step 2: Call #bsp_eint_key_set_debounce_time() to set the external interrupt debounce time.
 *   - Step 3: Call #bsp_eint_key_register_callback() to register a callback function to deal with the key event.
 *   - Step 4: Call #bsp_eint_key_enable() to enable the external interrupt of all eint key.
 *   - Sample code:
 *  @code
 *  static void bsp_eint_key_callback(bsp_eint_key_event_t event, uint8_t key_data, void *user_data)
 *  {
 *      // The incoming value of key_data aligns with the value in the file eint_key_custom.h.
 *      switch(key_data) {
 *      case BSP_EINT_KEY_DATA0:
 *          // Perform the action according to the specific event type.
 *          break;
 *
 *      case BSP_EINT_KEY_DATA1:
 *          // Perform the action according to the specific event type.
 *          break;
 *
 *      case default:
 *          break;
 *      }
 *  }
 *
 *  bsp_eint_key_config_t key_config;
 *  uint32_t debounce_time;
 *
 *  key_config.longpress_time = 0;
 *  key_config.repeat_time    = 0;
 *
 *  if (bsp_eint_key_init(&key_config) != true) {
 *     return false;
 *  }
 *
 *  debounce_time = 5;
 *  bsp_eint_key_set_debounce_time(debounce_time);
 *
 *  bsp_eint_key_register_callback(bsp_eint_key_callback, NULL);
 *
 *  bsp_eint_key_enable();
 *
 *  @endcode
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include "hal_gpio.h"
#include "hal_eint.h"


/** @defgroup eint_key_struct Struct
  * @{
  */

/** @brief This structure defines key timing configurations. */
typedef struct {
    uint32_t    longpress_time;     /**< longpress time setting. if it is set to zero, a default value of 2000 will be used. */
    uint32_t    repeat_time;        /**< repeat time setting. if it is set to zero, a default value of 1000 will be used. */
} bsp_eint_key_config_t;

/**
* @}
*/

/** @defgroup eint_key_enum Enums
  * @{
  */

/** @brief This enum defines key event status. */
typedef enum {
    BSP_EINT_KEY_RELEASE        = 0,        /**< A key is released. */
    BSP_EINT_KEY_PRESS          = 1,        /**< A key is pressed. */
    BSP_EINT_KEY_LONG_PRESS     = 2,        /**< A key is long pressed. */
    BSP_EINT_KEY_REPEAT         = 3,        /**< A key is repeat pressed. */
} bsp_eint_key_event_t;
/**
* @}
*/

/** @defgroup eint_key_typedef Typedef
  * @{
  */

/** @brief  This defines the callback function prototype.
 *          User should register a callback function when using eint key, this function will be called in external interrupt
 *          service routine after a key is pressed or released.
 *  @param [in] event is the key event for the current key status, user can get the key status from this parameter, please
 *              refer to #bsp_eint_key_event_t for details about the event type.
 *  @param [in] key_data is used to identify a specific key.
 *  @param [in] user_data is a user-defined parameter provided by #bsp_eint_key_register_callback().
 *  @sa  #bsp_eint_key_register_callback()
 */
typedef void (*bsp_eint_key_callback_t)(bsp_eint_key_event_t event, uint8_t key_data, void *user_data);

/**
* @}
*/

/**
 * @brief This function is used to initialize eint key module.
 *
 * @param [in] config is eint key timing config. see #bsp_eint_key_config_t .
 * @return true  the operation completed successfully.
 * @return false config is  NULL or eint key has been initialized.
 */
bool bsp_eint_key_init(bsp_eint_key_config_t *config);

/**
 * @brief This function is used to register eint key callback function, the callback function is called when eint interrupt is asserted.
 *
 * @param [in] callback is called when a key event must be reported to user.
 * @param [in] user_data is a parameter of the callback function.
 * @return true     register callback function successfully.
 * @return false callback is NULL.
 */
bool bsp_eint_key_register_callback(bsp_eint_key_callback_t callback, void *user_data);

/**
 * @brief This function is used to set eint key timing config.
 *
 * @param [in] config is eint key timing config. see #bsp_eint_key_config_t .
 * @return true     set config successfully.
 * @return false    config is NULL
 */
bool bsp_eint_key_set_event_time(bsp_eint_key_config_t *config);

/**
 * @brief This function is used to set debounce time of eint key in microsecond.
 *
 * @param [in] debounce_time is debounce time in microseconds.
 * @return true set debounce time for all eint key successfully.
 * @return false eint number error.
 */
bool bsp_eint_key_set_debounce_time(uint32_t debounce_time);

/**
 * @brief This function is used to enable interrupt of eint key.
 *
 * @return true  enable all eint key interrupt successfully.
 * @return false eint number error.
 */
bool bsp_eint_key_enable(void);

/**
 * @brief This function is used to simulate a key press event.
 *        if key is pressed, issue a simulation key event to upper layer. It is usually used to send a press
 *        event at power on procedure because a key press action can not be detected before external interrupt has been initialized.
 * @return none
 */
void bsp_eint_key_pressed_key_event_simulation(void);

/**
 * @brief This function is used to get the eint key status by key data.
 *
 * @param [in]  eint_key_data is eint key data, it must be the value one from BSP_EINT_KEY_DATA0 to BSP_EINT_KEY_DATA4.
 * @param [out] status is the key status. Please refer to #bsp_eint_key_event_t.
 * @return  0:success
 * @return -1:failure
 */
int bsp_eint_key_get_status(uint8_t eint_key_data, bsp_eint_key_event_t *status);

#ifdef __cplusplus
}
#endif
/**
* @}
* @}
*/
#endif /* _EINT_KEY_CUSTOM_H_ */

