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

#ifndef __AIRO_KEY_EVENT_H__
#define __AIRO_KEY_EVENT_H__

/**
 * @addtogroup airokey
 * @{
 * This section introduces the airokey APIs including terms and acronyms,
 * supported features, software architecture, details on how to use the airokey, enums, structures, typedefs and functions.
 *
 * @section airokey_Terms_Chapter Terms and acronyms
 *
 * |        Terms         |           Details                |
 * |----------------------|----------------------------------|
 * |\b airokey            | a flexible framework to realize many kinds of key events. |
 *
 *
 * @section airokey_Features_Chapter Supported features
 *
 * - **Support various modules**
 *   - Airokey supports several kinds of key types, such as gsensor, eintkey, captouch, powerkey, and so on.
 * - **Easy to expand**
 *   - It is easy to expand other types of key modules in airokey, as long as this module supports press & release events.
 *   - It is easy to expand the event type, such as expanding the click level to a quad-click or penta-click, or changing a long press event to a different time level.
 *
 * @section airokey_APIs_Usage_Chapter How to use the airokey APIs?
 *
 * - To use the airokey services follow the steps, as shown below. \n
 *  - Step 1. Copy middleware/MTK/key/template_config/airo_key_config.h to user's project path.
 *  - Step 2. Include \$(SOURCE_DIR)/middleware/MTK/key/module.mk in user's Makefile.
 *      - If user wants to use eintkey, include \$(SOURCE_DIR)/driver/board/\$(BOARD_CONFIG)/eint_key/module.mk in user's Makefile.
 *      - If user wants to use gsensor, include \$(SOURCE_DIR)/driver/board/component/gsensor_key/module.mk in user's Makefile.
 *      - If user wants to use captouch, the macro HAL_CAPTOUCH_MODULE_ENABLED must be defined in hal_feature_config.h.
 *      - If user wants to use powerkey, the macro HAL_PMU_MODULE_ENABLED must be defined in hal_feature_config.h, because powerkey
 *        relies on the PMU module. The three-line comments below must be removed from airo_key_config.h.
 *    @code
 *        //#ifdef HAL_PMU_MODULE_ENABLED
 *        //#define AIRO_KEY_FEATRURE_POWERKEY
 *        //#endif
 *    @endcode
 *  - Step 3. Call airo_key_event_init() to initialize the airokey module.
 *  - Step 4. Implement a callback function in airo_key_callback_t type to process all kinds of key events.
 *  - Step 5. Call airo_key_register_callback() to register the callback function.
 *  - Sample code:
 *    @code
 *       void airo_key_eint_key_callback(airo_key_event_t event, uint8_t key_data, void *user_data)
 *       {
 *          // The incoming value of key_data aligns with the value in the file airo_key_config.h, which is defined by user.
 *          switch(key_data) {
 *          case EINT_KEY_0:
 *              // Perform the action according to the specific event type.
 *              break;
 *
 *          case DEVICE_POWER_KEY:
 *              // Perform the action according to the specific event type.
 *              break;
 *
 *          case default:
 *              break;
 *          }
 *       }
 *       ret = airo_key_event_init();
 *       if (ret == false) {
 *          // Perform the error-handling operation.
 *       }
 *       ret = airo_key_register_callback(airo_key_eint_key_callback, NULL);
 *       if (ret == false) {
 *          // Perform the error-handling operation.
 *       }
 *    @endcode
 *
 * @section airokey_Customization_Chapter How to customize keys?
 *  - To customize keys, the user must consider the following three things:
 *    - How to specify the key numbers that the user wants to use?
 *    - How to identify a key?
 *       - key_data is used to identify a key, when user receives an event from the callback function, they can
 *         refer to key_data to know which key the event belongs to.
 *       - The macro AIRO_KEY_MAPPING, which is defined in airo_key_config.h, contains parameters of the keys of
 *         all key modules which are defined by user, except for the gsensor module. AIRO_GSENSOR_KEY_MAPPING contains
 *         the gsensor settings. Each element of AIRO_KEY_MAPPING or AIRO_GSENSOR_KEY_MAPPING is airo_key_event_config_t type,
 *         its first value is key_data and can be customized by user.
 *    - How to enable the hardware?
 *
 *  - Details about customizing keys. \n
 *
 *   - eintkey \n
 *     - Key numbers \n
 *        The hardware supported key numbers are determined by the macro BSP_EINT_KEY_NUMBER which is defined in the file
 *        driver/board/\<board type\>/eint_key/inc/eint_key_custom.h. The maximum value of BSP_EINT_KEY_NUMBER
 *        is currently four. The user does not have to change the value if the number of eintkey is less than four, it shows
 *        the ability of hardware to support the numbers of keys, in other words, although there are four keys defined in
 *        eint_key_custom.h. The user can configure any number of keys (up to four) in airo_key_config.h.
 *     - key_data \n
 *        There are two places to specify the key_data: one is driver/board/\<board type\>/eint_key/inc/eint_key_custom.h,
 *        we named it as A; the other place is middleware/MTK/key/template_config/airo_key_config.h, we named it as B.
 *        key_data in position A shows the ability of hardware to support the numbers of keys. key_data in position B shows
 *        the user's configuration. Its value must align with the key_data in position A.
 *     - Hardware function configuration \n
 *        User must use the EPT tool to configure the GPIO functions. This tool generates several files, including ept_gpio_var.c
 *        and ept_eint_var.c. The user must copy these files to the project folder. driver/board/\<board type\>/eint_key/src/eint_key_custom.c
 *        must have the variables from these files, including BSP_EINTKEYx_PIN, BSP_EINTKEYx_PIN_M_EINT and BSP_EINTKEYx_EINT (x=0/1/2/3),
 *        to configure GPIOs.
 *
 *   - gsensor \n
 *     - Key numbers \n
 *        Only one key is supported. Two types of sensor are supported, including LIS2DS12 and LIS2DW12. User can switch the
 *        sensor by defining a macro, GSENSOR_LIS2DW12_EANBLE or GSENSOR_LIS2DS12_EANBLE in the file driver/board/component/gsensor_key/module.mk.
 *     - key_data \n
 *        It is specified by a macro AIRO_CONFIG_GSENSOR_KEY, which is defined in middleware/MTK/key/template_config/airo_key_config.h.
 *     - Hardware function configuration \n
 *        No additional hardware function configuration is required.
 *
 *   - captouch \n
 *     - Key numbers \n
 *        The number of hardware supported keys is eight. The user must use the EPT tool to configure which key is used, This tool generates
 *        several files, including ept_keypad_drv.h, each bit of macro DRV_KBD_CAPTOUCH_SEL in ept_keypad_drv.h represents a key.
 *     - key_data \n
 *        There are two places to specify the key_data: one is the macro KEYPAD_MAPPING in file ept_keypad_drv.h, which is generated
 *        by the EPT tool, we named it as A. The other place is middleware/MTK/key/template_config/airo_key_config.h, we named it
 *        as B. key_data in position A shows the ability of hardware to support the numbers of keys. key_data in position B shows
 *        the user's configuration. Its value must align with the key_data in position A.
 *     - Hardware function configuration \n
 *        No additional hardware function configuration is required. The user must make sure that the macro DRV_KBD_CAPTOUCH_SEL can
 *        not be 0, or none of the captouch keys will operate as expected.
 *
 *   - powerkey
 *     - Key numbers \n
 *        Only one key is supported.
 *     - key_data \n
 *        There are two places to specify the key_data: one is a macro POWERKEY_POSITION in file ept_keypad_drv.h, which is generated
 *        by the EPT tool, we named it as A. The other place is middleware/MTK/key/template_config/airo_key_config.h, we named it
 *        as B. key_data in position A shows the ability of hardware to support the numbers of keys. key_data in position B shows
 *        the user's configuration. Its value must align with the key_data in position A.
 *     - Hardware function configuration \n
 *        No additional hardware function configuration is required. The powerkey module is used to expand the function of the power key
 *        itself; it sets the power key as a normal key.
 */


#include "stdint.h"
#include "stdbool.h"
#include "airo_key_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup airokey_define Define
  * @{
  */

/** @brief The event type mask of the airokey framework. */
#define AIRO_KEY_TYPE_MASK                    0xF0
/** @brief The click event type mask of the airokey framework. */
#define AIRO_KEY_CLICK_TYPE_MASK              0x00
/** @brief The long press event type mask of the airokey framework. */
#define AIRO_KEY_LONG_PRESS_TYPE_MASK         0x20
/** @brief The long press release event type mask of the airokey framework. */
#define AIRO_KEY_LONG_PRESS_RELEASE_TYPE_MASK 0x50

/**
  * @}
  */


/** @defgroup airokey_enum Enum
  * @{
  */

/** @brief All the supported events of the airokey framework. */
typedef enum {
    AIRO_KEY_RELEASE              = 0x00,                                       /**< A key is hardware released.*/
    AIRO_KEY_SHORT_CLICK          = AIRO_KEY_CLICK_TYPE_MASK  | 1,              /**< A key is short-clicked. */
    AIRO_KEY_DOUBLE_CLICK         = AIRO_KEY_CLICK_TYPE_MASK  | 2,              /**< A key is double-clicked */
    AIRO_KEY_TRIPLE_CLICK         = AIRO_KEY_CLICK_TYPE_MASK  | 3,              /**< A key is triple-clicked. */
    AIRO_KEY_LONG_PRESS_1         = AIRO_KEY_LONG_PRESS_TYPE_MASK | 1,          /**< A key is long pressed. */
    AIRO_KEY_LONG_PRESS_2         = AIRO_KEY_LONG_PRESS_TYPE_MASK | 2,          /**< A key is long pressed. */
    AIRO_KEY_LONG_PRESS_3         = AIRO_KEY_LONG_PRESS_TYPE_MASK | 3,          /**< A key is long pressed. */
    AIRO_KEY_SLONG                = 0x30,                                       /**< A key is short-clicked and then long pressed. */
    AIRO_KEY_DLONG                = 0x31,                                       /**< A key is double-clicked and then long pressed. */
    AIRO_KEY_REPEAT               = 0x38,                                       /**< A key remains pressed after the last long press event is issued. */
    AIRO_KEY_PRESS                = 0x40,                                       /**< A key is hardware pressed.*/

    AIRO_KEY_LONG_PRESS_RELEASE_1 = AIRO_KEY_LONG_PRESS_RELEASE_TYPE_MASK | 1,  /**< A special event, a key is released after AIRO_KEY_LONG_PRESS_1.
                                                                                    This event will not be used in state machine. */
    AIRO_KEY_LONG_PRESS_RELEASE_2 = AIRO_KEY_LONG_PRESS_RELEASE_TYPE_MASK | 2,  /**< A special event, a key is released after AIRO_KEY_LONG_PRESS_2.
                                                                                    This event will not be used in state machine. */
    AIRO_KEY_LONG_PRESS_RELEASE_3 = AIRO_KEY_LONG_PRESS_RELEASE_TYPE_MASK | 3,  /**< A special event, a key is released after AIRO_KEY_LONG_PRESS_3.
                                                                                    This event will not be used in state machine. */
    AIRO_KEY_SLONG_RELEASE        = 0x60,                                       /**< A special event, a key is released after AIRO_KEY_SLONG.
                                                                                    This event will not be used in state machine. */
    AIRO_KEY_DLONG_RELEASE        = 0x61,                                       /**< A special event, a key is released after AIRO_KEY_DLONG.
                                                                                    This event will not be used in state machine. */

    AIRO_KEY_INVALID              = 0xFF,                                       /**< A key is pressed but it does not match any event.*/
} airo_key_event_t;

/**
  * @}
  */

/** @defgroup airokey_struct Struct
  * @{
  */

/** @brief The event support configuration structure of the airokey framework. */
typedef struct {
    uint8_t press_release;             /**< 1: Support hardware press and release event. \n
                                            0: No support.
                                        */

    uint8_t slong;                     /**< If support short long event, the long press level should be at least one.\n
                                            Short long event indicates that a long press event occurs within a
                                            specified period of time after a short-click. \n
                                            1: Support short long event. \n
                                            0: No support.\n
                                        */

    uint8_t slong_repeat;              /**< 1: Repeat event. \n
                                            0: No support.
                                        */

    uint8_t long_level;                /**< Supported long press level, the max level is 3, user can extend the max level. \n
                                            0: No support.
                                        */

    uint8_t long_repeat;               /**< Supported long repeat. \n
                                            1: Support repeat event after long press event. \n
                                            0: No support.
                                        */

    uint8_t multiple_click;            /**< Supported multiple clicks, the max level is 3, user can extend the max level. \n
                                            0: No support.
                                        */
} airo_key_support_event_t;

/** @brief The event time configuration structure of the airokey framework, the unit is millisecond. */
typedef struct {
    uint32_t t_press;                    /**< The press time of the click. \n
                                            In the case of a multiple click, if the long press event is supported, the first press time is t_long_press_1.
                                            If the short long press event is supported, the second press time is t_slong. Otherwise, t_press will be used.
                                        */

    uint32_t t_release;                 /**< The release time is the period of time to wait for the next click, only for multiple click cases. */

    uint32_t t_silence;                 /**< The silence time, only for click cases. \n
                                            For example, if a key only supports double-click, when the double-click event is sent, it will enter the silence time.
                                            During silence time, the driver will not process any unnecessary clicks. The silence time restarts when a new release occurs.\n
                                            The key can only enter the next event state when the silence time expires.
                                        */

    uint32_t t_repeat;                  /**< The repeat event time.\n
                                            If repeat event is supported, it is reported after long press event or short long press event.
                                        */

    uint32_t t_long_press_1;            /**< The first level of long press time. */
    uint32_t t_long_press_2;            /**< The second level of long press time. */
    uint32_t t_long_press_3;            /**< The third level of long press time. */
    uint32_t t_slong;                   /**< The short long press time. */
} airo_key_event_time_t;

/** @brief The event configuration structure of the airokey framework. */
typedef struct {
    uint32_t                key_data;   /**< The key data is a label for a hardware key, it is a unique identification symbol, one key only has one key data value. \n
                                            The valid value of key_data is defined in the file "driver\chip\<IC series>\inc\hal_keypad_table.h". \n
                                        */
    airo_key_event_time_t    time;      /**< Please refer to the comments in airo_key_event_time_t */
    airo_key_support_event_t support;   /**< Please refer to the comments in airo_key_support_event_t */
} airo_key_event_config_t;

#ifdef MTK_GSENSOR_KEY_ENABLE
/** @brief The gsensor configuration structure. */
typedef struct {
    uint8_t  key_data;                  /**< Please refer to the comments of key_data in airo_key_event_time_t */
    uint8_t  multiple_click;            /**< It is the number of multiple clicks that can be supported. This value cannot be zero. */
    uint32_t t_interval;                /**< The maximal interval time for recognizing multiple clicks as double-clicks or triple-clicks.*/
    uint32_t t_silence;                 /**< Please refer to the comments of t_silence in airo_key_event_time_t. */
} airo_key_gsensor_config_t;
#endif
/**
  * @}
  */

/** @defgroup airokey_typedef Typedef
  * @{
  */

/**
 * @brief  The type of airokey callback function.
 *
 * @param[in] event means that whatever happened to a key. It must be processed by the user.
 * @param[in] key_data is used to identify a specific key.
 *            please refer to the comments in airo_key_event_config_t for more details.
 * @param[in] user_data is passed in by airo_key_register_callback function when this type
 *            of callback function is registered.
 * @return    None
 * @note      The user should not do anything that consumes too much time in the callback function.
 *            Because this callback function is in the ISR level, we suggest that the user distributes
 *            the events to tasks for subsequent processing in the application with an OS.
*/
typedef void (*airo_key_callback_t)(airo_key_event_t event, uint8_t key_data, void *user_data);
/**
  * @}
  */

/**
 * @brief     Airokey framework initialization function. The user must first call this function.
 *
 * @return    true means that it successfully initialized the airokey framework. \n
 *            false means it failed to initialize the airokey framework; this function has been called before. \n
 */
bool airo_key_event_init(void);

/**
 * @brief     Register a callback function in the airokey framework.
 *
 * @param[in] callback will be called when a key event must be reported to user.
 * @param[in] user_data is a parameter of the callback function.
 * @return    true means the callback function is registered successfully. \n
 *            false means the parameter callback may be NULL. \n
 * @note      This function must be called after #airo_key_event_init().
 */
bool airo_key_register_callback(airo_key_callback_t callback, void *user_data);

/**
 * @brief     Simulate a key event, it's a function for debugging.
 *
 * @param[in] event means what happened to a key. It must be processed by the user.
 * @param[in] key_data is used to identify a specific key.
 *            please refer to the comments in airo_key_event_config_t to get more details.
 * @return    None
 */
void airo_key_send_simulation_event(airo_key_event_t event, uint8_t key_data);

/**
 * @brief      Get the event time by key data
 *
 * @param[in]  key   Please refer to the comments of key_data in airo_key_event_config_t.
 * @param[out] time  Please refer to the comments in airo_key_event_time_t.
 * @return     true  the time is found.
 *             false the time is not found, the key is not in the user's configuration.
 */
bool airo_key_get_event_time(uint32_t key, airo_key_event_time_t *time);

/**
 * @brief      Set the event time by key data
 *
 * @param[in]  key   Please refer to the comments of key_data in airo_key_event_config_t.
 * @param[in]  time  Please refer to the comments in airo_key_event_time_t.
 * @return     true  the time is set correctly.
 *             false the time is not set correctly, the key is not in the user's configuration.
 * */
bool airo_key_set_event_time(uint32_t key, airo_key_event_time_t *time);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __AIRO_KEY_EVENT_H__ */
