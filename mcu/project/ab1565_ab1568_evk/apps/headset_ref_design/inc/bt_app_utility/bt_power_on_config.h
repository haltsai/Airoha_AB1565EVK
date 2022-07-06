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

/**
 * File: bt_power_on_config.h
 *
 * Description: The file defines the interface of bt_power_on_config.c.
 *
 */

#ifndef __BT_POWER_ON_CONFIG_H__
#define __BT_POWER_ON_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOSConfig.h"

#define PACKED __attribute__((packed))

/**
*  @brief A structure to represent bt power on relay mode configration.
*/
typedef struct {
    bool relay_enable;      /**< Is enable relay mode */
    uint8_t port_number;    /**< The UART port to relay */
} PACKED bt_power_on_relay_config_t;

/**
*  @brief A enumeration to represent bt poewr on mode.
*/
typedef enum {
    BT_POWER_ON_NORMAL = 0,  /**< BT power on normal mode */
    BT_POWER_ON_RELAY,       /**< BT power on relay mode */
    BT_POWER_ON_DUT,         /**< BT power on dut mode */
} bt_power_on_type_t;

/**
 * @brief     This function is to init bt power on configure when system power on.
 * @return    void.
 */
void bt_power_on_config_init(void);

/**
 * @brief     This function is to get bt power on config type.
 * @return    #BT_POWER_ON_NORMAL, bt power on mode is default normal mode.
 *            #BT_POWER_ON_RELAY, bt power on mode is relay mode.
              #BT_POWER_ON_DUT, bt power on mode is dut mode.
 */
bt_power_on_type_t bt_power_on_get_config_type(void);

/**
 * @brief     This function is to set bt power on config type.
 * @param[in] power_on_type     The target mode to config bt power on mode.
 * @return    void.
 */
void bt_power_on_set_config_type(bt_power_on_type_t power_on_type);

/**
 * @brief     This function is to get the bt relay UART port number.
 * @return    BT relay UART port number.
 */
uint8_t bt_power_on_get_relay_port_number(void);

/**
 * @brief     This function defines the bt controller test task.
 * @param[in] param     The parametes to pass in when the function start to run.
 * @return    void.
 */
void bt_controler_test_task(void *param);

#endif

