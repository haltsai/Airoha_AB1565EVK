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

#ifndef __BSP_LED_ISINK_H__
#define __BSP_LED_ISINK_H__
#ifdef HAL_ISINK_MODULE_ENABLED
#ifdef __cplusplus
extern "C" {
#endif
#include "hal_platform.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hal.h"
#include "bsp_led.h"

/**
 * @brief The enumeration of led channel.
 *
 */
typedef enum {
    BSP_LED_CHANNEL_0, /**< The led channel 0 */
    BSP_LED_CHANNEL_1, /**< The led channel 1 */
    BSP_LED_CHANNEL_MAX/**< The led error channel */
} bsp_led_channel_t;

/**
 * @brief The enumeration of led state machine.
 *
 */
typedef enum {
    BSP_LED_STATE_IDLE = 0, /**< The led state in idle */
    BSP_LED_STATE_T0,       /**< The led state in T0 */
    BSP_LED_STATE_T1T2_RPT, /**< The led state in T1T2 repeat */
    BSP_LED_STATE_T3,       /**< The led state in T3 */
    BSP_LED_STATE_MAX
} bsp_led_state_t;

/**
 * @brief The enumeration of led operate action.
 *
 */
typedef enum {
    BSP_LED_ACTION_OF_START,        /**< The led start action for led */
    BSP_LED_ACTION_OF_TIMEOUT,      /**< The led timeout action for led */
    BSP_LED_ACTION_OF_STOP,         /**< The led stop action for led */
    BSP_LED_ACTION_OF_CALI_TIMEOUT, /**< The led cali timeout action for led */
    BSP_LED_ACTION_MAX
} bsp_led_action_t;

/**
 * @brief The struct define the calibration information for led cali.
 *
 */
typedef struct {
    uint32_t    expected_expire_tick;    /**< The tick of led expected timeout base on 32k gpt */
    float       led_sw_cali_coefficient; /**< The led cali coefficient */
} bsp_led_sw_cali_t;

/**
 * @brief The struct define the led private information for led control.
 *
 */
typedef struct {
    uint8_t             led_id;       /**< The led number specifies to operate. */
    bsp_led_mode_t      mode;         /**< The mode led used. */
    bsp_led_config_t    led_cfg;       /**< The mode led used. */
    bsp_led_state_t     cur_state;     /**< The led current state. */
    bsp_led_action_t    next_action;   /**< The led next action. */
    bool                req_cali;      /**< For furture use. */
    bsp_led_sw_cali_t   sw_cali;       /**< SW cali information. */
    uint32_t            gpt_handle;    /**< The GPT handle that use. */
    uint32_t            t1t2_cycle;    /**< The t1t2 cycle. */
    uint32_t            ext_loop_cycle;/**< The extent loop cycle. */
} bsp_isink_private_info_t;

/**
 * @brief This function for initialize the led state machine and HW.
 * @param[in] ledx specifies the led number to operate.
 * @param[in] mode is the configuration for led operate mode.
 * @return #BSP_LED_STATUS_OK, the operation completed successfully.
 */
bsp_led_status_t    bsp_led_isink_init(uint8_t ledx, bsp_led_mode_t mode);

/**
 * @brief This function for de-initialize the led state machine and HW.
 * @param[in] ledx specifies the led number to operate.
 * @return #BSP_LED_STATUS_OK, the operation completed successfully.
 */
bsp_led_status_t    bsp_led_isink_deinit(uint8_t ledx);

/**
 * @brief This function for configure the led hw.
 * @param[in] ledx specifies the led number to operate.
 * @param[in] cfg is the configuration for led timing and anothers setting.
 * @return #BSP_LED_STATUS_OK, the operation completed successfully.
 */
bsp_led_status_t    bsp_led_isink_config(uint8_t ledx, bsp_led_config_t *cfg);

/**
 * @brief This function start led.
 * @param[in] ledx specifies the led number to operate.
 * @return #BSP_LED_STATUS_OK, the operation completed successfully.
 */
bsp_led_status_t    bsp_led_isink_start(uint8_t ledx);

/**
 * @brief This function suspend led.
 * @param[in] ledx specifies the led number to operate.
 * @return #BSP_LED_STATUS_OK, the operation completed successfully.
 */
bsp_led_status_t    bsp_led_isink_stop(uint8_t ledx);

/**
 * @brief This function suspend led.
 * @param[in] ledx specifies the led number to operate.
 * @param[in] cmd is the command.
 * @param[in] option is a additation parameter for user using.
 * @return the result of this operate.
 */
int                 bsp_led_isink_ioctrl(uint32_t ledx, uint32_t cmd, uint32_t option);


#ifdef __cplusplus
}
#endif

#endif /* HAL_ISINK_MODULE_ENABLED */
#endif /* __BSP_LED_ISINK_H__ */



