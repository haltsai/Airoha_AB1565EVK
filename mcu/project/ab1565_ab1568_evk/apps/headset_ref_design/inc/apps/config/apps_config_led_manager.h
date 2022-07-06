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

#ifndef __APPS_CONFIG_LED_MANAGER_H__
#define __APPS_CONFIG_LED_MANAGER_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce.h"
#include "bt_aws_mce_report.h"
#endif

/** @brief
 * This enum defines event id for group EVENT_GROUP_UI_SHELL_LED_MANAGER.
 */
enum {
    APPS_EVENTS_LED_FG_PATTERN_TIMEOUT, /**< Internal use, LED foreground pattern has timeout category, when timeout happen, app_preproc_activity
                                                       receives the event to disable LED foreground pattern. */
#ifdef MTK_AWS_MCE_ENABLE
    APPS_EVENTS_LED_SYNC_LED_PATTERN,   /**< Internal use, notify a aws mce report for LED sync, when led sync message arrives, app_preproc_activity
                                                       receives the event to process the message */
#endif
};

/** @brief
 * This enum defines the priority of LED patterns.
 * If there is a background pattern are synced from agent, LED module compares the priority of
 * the local pattern and synced pattern.
 */
typedef enum {
    APPS_CONFIG_LED_AWS_SYNC_PRIO_INVALID,  /**< Invalid priority value. It is used to initialize vairable. */
    APPS_CONFIG_LED_AWS_SYNC_PRIO_LOWEST,   /**< The priority is lowest. */
    APPS_CONFIG_LED_AWS_SYNC_PRIO_LOW,      /**< The priority is lowest. */
    APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE,   /**< The priority is middle. */
    APPS_CONFIG_LED_AWS_SYNC_PRIO_HIGH,     /**< The priority is high. */
    APPS_CONFIG_LED_AWS_SYNC_PRIO_HIGHEST   /**< The priority is highest. */
} apps_config_led_manager_aws_sync_priority_t;

/**
 * @brief Internal use only. To avoid the crash issue that caused by calling LED function in
 * timer callback, sends a event in timer callback when foreground LED pattern timeout. After
 * the pre-proc activity receives the event, it calls the function to disable LED pattern.
 * @return None
 */
void apps_config_check_foreground_led_pattern(void);

/**
 * @brief APPs call the function to set a background LED pattern.
 * @param[in] index is the pattern index.
 * @param[in] need_sync true means need sync to partern. If it's partner calls the function
 *            and it attaches to agent, the setting will be pending util it detaches.
 * @param[in] priority is the priority of the LED pattern.
 * @return true means set pattern successfully.
 */
bool apps_config_set_background_led_pattern(uint8_t index, bool need_sync, apps_config_led_manager_aws_sync_priority_t priority);

/**
 * @brief APPs call the function to set a forground LED pattern.
 * @param[in] index is the pattern index.
 * @param[in] timeout is the duration of the LED pattern is effective.
 * @param[in] need_sync true means need sync to partern. If it's partner calls the function
 *            and it attaches to agent, the setting will be pending util it detaches.
 * @return true means set pattern successfully.
 */
bool apps_config_set_foreground_led_pattern(uint8_t index, uint16_t timeout, bool need_sync);

#ifdef MTK_AWS_MCE_ENABLE
/**
 * @brief APPs call the function to notify aws attached.
 * @param[in] attached is true if the event is attached and it is false if the event is detached.
 * @param[in] role is the AWS role of the device.
 * @return none.
 */
void apps_config_led_manager_on_aws_attached(bt_aws_mce_role_t role, bool attached);
#endif

/**
 * @brief call the function when system initializing.
 * @return none.
 */
void apps_config_led_manager_init(void);

#ifdef MTK_AWS_MCE_ENABLE
/**
 * @brief APPs call the function to set led pattern or do led sync when receives led sync aws report.
 * @param[in] param, is the pointer to the "void *param" of bt_aws_mce_report_info_t.
 * @return none.
 */
void app_config_led_sync(void *param);
#endif

/**
 * @brief APPs call the function to disable LED temporarily.
 * @param[in] disable_led is true to disable LED and false to recovery.
 * @return none.
 */
void apps_config_led_temporary_disable(bool disable_led);

/**
 * @brief The function can disable all LED pattern and clear status, normally called before power off.
 * @return none.
 */
void apps_config_led_disable_all(void);

#endif /* __APPS_CONFIG_LED_MANAGER_H__ */
