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


#ifndef __BSP_ROTARY_ENCODER_H__
#define __BSP_ROTARY_ENCODER_H__



/**
 * @addtogroup BSP
 * @{
 * @addtogroup Rotary_Encoder
 * @{
 * This section describes the programming interfaces of the rotary encoder driver, which provides the driver to report basic events to the upper layer.
 *     ----> Clockwise
 *             _________         _________
 * PinA ______|        |________|        |______
 *                 _________         _________
 * PinB __________|        |________|        |__
 *         00  01  11  10   00   01   11  10
 *      <---- Couter Clockwise
 *
 *
 *  -------------------------------------
 *  |   Rotary Encoder direction table  |
 *  |-----------------------------------|
 *  |Previous |Current  |          |    |
 *  |PinB|PinA|PinB|PinA|Direction |Mark|
 *  | 0  | 0  | 0  | 0  | NA       | 0  |
 *  | 0  | 0  | 0  | 1  | CW       | 1  |
 *  | 0  | 0  | 1  | 0  | CCW      | -1 |
 *  | 0  | 0  | 1  | 1  | NA       | 0  |
 *  | 0  | 1  | 0  | 0  | CCW      | -1 |
 *  | 0  | 1  | 0  | 1  | NA       | 0  |
 *  | 0  | 1  | 1  | 0  | NA       | 0  |
 *  | 0  | 1  | 1  | 1  | CW       | 1  |
 *  | 1  | 0  | 0  | 0  | CW       | 1  |
 *  | 1  | 0  | 0  | 1  | NA       | 0  |
 *  | 1  | 0  | 1  | 0  | NA       | 0  |
 *  | 1  | 0  | 1  | 1  | CCW      | -1 |
 *  | 1  | 1  | 0  | 0  | NA       | 0  |
 *  | 1  | 1  | 0  | 1  | CCW      | -1 |
 *  | 1  | 1  | 1  | 0  | CW       | 1  |
 *  | 1  | 1  | 1  | 1  | NA       | 0  |
 *  -------------------------------------
 * @section Rotary_Encoder_High_Level_Driver_Usage_Chapter How to use the rotary encoder
 *  The steps are shown below:
 *   - Step 1: Call #bsp_rotary_encoder_init() to initialize the rotary encoder module.
 *   - Step 2: Call #bsp_rotary_encoder_enable() to enable the external interrupt of all rotary encoder.
 *
 *   - Sample code:
 *  @code
 * static void bsp_rotary_encoder_callback(bsp_rotary_encoder_port_t port, bsp_rotary_encoder_event_t event, uint32_t position, void *user_data)
 *  {
 *      uint8_t *event_type[2] = {"CW","CCW"};
 *
 *      printf("[%s]port=%d key event=%s user_data=%d", __func__, port, event_type[event], position);
 *  }
 *
 *   bsp_rotary_encoder_status_t status;
 *   bsp_rotary_encoder_config_t config;
 *   uint32_t user_data;
 *
 *   config.callback = bsp_rotary_encoder_callback;
 *   config.eint_debounce_time_ms = 10;
 *   config.user_data = (void*)&user_data;
 *   config.event_threshold = 4;
 *   config.event_timeout_ms = 100;
 *
 *   status = bsp_rotary_encoder_init(BSP_ROTARY_ENCODER_0, &config);
 *   if (status != BSP_ROTARY_ENCODER_STATUS_OK) {
 *       printf("rotary init fail = %d\r\n", status);
 *   }
 *  @endcode
 */

#include "hal.h"
#include "hal_gpio.h"

/** @defgroup Rotary_Encoder_Enum Enum
  * @{
  */
/**
 * @brief This enum defines the avaliable rotary encoder port.
 */
typedef enum {
    BSP_ROTARY_ENCODER_0,  /**< rotary encoder port 0. */
    BSP_ROTARY_ENCODER_1,  /**< rotary encoder port 1. */
    BSP_ROTARY_ENCODER_2,  /**< rotary encoder port 2. */
    BSP_ROTARY_ENCODER_MAX,/**< rotary encoder port max value. */
} bsp_rotary_encoder_port_t;

/**
 * @brief This enum defines the event type of rotary encoder.
 */
typedef enum
{
    BSP_ROTARY_ENCODER_RELATIVE_CHANGE = 0, /**< The event data is relative change value. */
    BSP_ROTARY_ENCODER_ABSOLUTE_CHANGE = 1  /**< The event data is absolute change value. */
} bsp_rotary_encoder_event_type_t;

/**
 * @brief This enum defines the event direction of rotary encoder.
 */
typedef enum {
    BSP_ROTARY_ENCODER_EVENT_CW  = 0, /**< The rotary encoder rotates clockwise. */
    BSP_ROTARY_ENCODER_EVENT_CCW = 1, /**< The rotary encoder rotates counter clockwise. */
} bsp_rotary_encoder_event_t;

/**
 * @brief This enum defines the status of rotary encoder.
 *
 */
typedef enum {
    BSP_ROTARY_ENCODER_STATUS_ERROR_INVALID_PARAMETER   = -2,  /**< The rotary encoder parameter error. */
    BSP_ROTARY_ENCODER_STATUS_ERROR                     = -1,  /**< The rotary encoder status error. */
    BSP_ROTARY_ENCODER_STATUS_OK                        = 0    /**< The rotary encoder status OK. */
} bsp_rotary_encoder_status_t;
/**
  * @}
  */

/** @defgroup Rotary_Encoder_typedef Typedef
  * @{
  */

/**
 * @brief This defines the callback function prototype.
 *      This function is called while the rotation of rotary encoder is detected.
 *
 * @param port       [in] is the rotary encoder port number, the value is defined in #bsp_rotary_encoder_port_t.
 * @param event      [in] is the event which indicate the direction of the rotary encoder.
 * @param event_data [in] is the current position of the rotary encoder.
 * @param user_data  [in] is a user-defined parameter.
 */
typedef void (*bsp_rotary_encoder_callback_t) (bsp_rotary_encoder_port_t port, bsp_rotary_encoder_event_t event, uint32_t event_data, void *user_data);

/**
  * @}
  */

/** @defgroup Rotary_Encoder_Struct Struct
  * @{
  */
/**
 * @brief
 *
 */
typedef struct {
    uint32_t eint_debounce_time_ms;             /**< external interrupt debounce time. */
    uint32_t event_threshold;                   /**< rotery encoder will report event to user when event counts is larger than or equal to event_threshold. */
    uint32_t event_timeout_ms;                  /**< when the event count is less than threshold, and there is no event in the time event_timeout_ms, the event will report to user.*/
    bsp_rotary_encoder_callback_t callback;     /**< call back for user receive event*/
    void *user_data;                            /**< A user-defined parameter */
} bsp_rotary_encoder_config_t;

/**
  * @}
  */


/**
 * @brief This function is used to initialize the rotary encoder.
 *
 * @param port    [in] is the rotary encoder port number, the value is defined in #bsp_rotary_encoder_port_t.
 * @param config  [in] is the configuration parameter of rotary encoder, please refer to #bsp_rotary_encoder_config_t.
 * @return #BSP_ROTARY_ENCODER_STATUS_ERROR_INVALID_PARAMETER means one of the parameters is error.
 *         #BSP_ROTARY_ENCODER_STATUS_OK means this function returns successfully.
 */
bsp_rotary_encoder_status_t bsp_rotary_encoder_init(bsp_rotary_encoder_port_t port, bsp_rotary_encoder_config_t *config);

/**
 * @brief This function is used to de-initialize the rotary encoder.
 *
 * @param port [in] is the rotary encoder port number, the value is defined in #bsp_rotary_encoder_port_t.
 * @return #BSP_ROTARY_ENCODER_STATUS_ERROR_INVALID_PARAMETER means one of the parameters is error.
 *         #BSP_ROTARY_ENCODER_STATUS_OK means this function returns successfully.
 */
bsp_rotary_encoder_status_t bsp_rotary_encoder_deinit(bsp_rotary_encoder_port_t port);

/**
 * @brief  This function is used to set the event threshold and timeout, which are used to control the event reports rate.
 *
 * @param port              [in] is the rotary encoder port number, the value is defined in #bsp_rotary_encoder_port_t.
 * @param event_threshold   [in] is the event threshold, please refer to #bsp_rotary_encoder_config_t.
 * @param event_timeout_ms  [in] is the event timeout, please refer to #bsp_rotary_encoder_config_t.
 * @return #BSP_ROTARY_ENCODER_STATUS_ERROR_INVALID_PARAMETER means one of the parameters is error.
 *         #BSP_ROTARY_ENCODER_STATUS_OK means this function returns successfully.
 */
bsp_rotary_encoder_status_t bsp_rotary_encoder_change_config(bsp_rotary_encoder_port_t port, uint32_t event_threshold, uint32_t event_timeout_ms);

#endif
