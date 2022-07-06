/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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
 * File: apps_events_event_group.h
 *
 * Description: This file defines the enum of event groups.
 *
 */

#ifndef __APPS_EVENTS_EVENT_GROUP_H__
#define __APPS_EVENTS_EVENT_GROUP_H__

#include "ui_shell_activity.h"

/** @brief
 * This enum defines the event group number in app.
 */
typedef enum {
    EVENT_GROUP_UI_SHELL_APP_INTERACTION = EVENT_GROUP_UI_SHELL_APP_BASE,   /**< group for interaction between apps */
    EVENT_GROUP_UI_SHELL_KEY,               /**< group for key events */
    EVENT_GROUP_UI_SHELL_ROTARY_ENCODER,    /**< group for rotary encoder events */
    EVENT_GROUP_UI_SHELL_BATTERY,           /**< group for battery events */
    EVENT_GROUP_UI_SHELL_BT,                /**< group for bt events */
    EVENT_GROUP_UI_SHELL_BT_SINK,           /**< group for bt sink events */
    EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER,   /**< group for bt connection manager */
    EVENT_GROUP_UI_SHELL_BT_DEVICE_MANAGER, /**< group for bt device manager */
    EVENT_GROUP_UI_SHELL_LE_SERVICE,        /**< group for bt le service event */
    EVENT_GROUP_BT_ULTRA_LOW_LATENCY,       /**< group for BT ultra_low_latency */
    EVENT_GROUP_UI_SHELL_FOTA,              /**< group for fota events */
    EVENT_GROUP_UI_SHELL_CHARGER_CASE,      /**< group for charger case events */
    EVENT_GROUP_UI_SHELL_AWS,               /**< group for AWS events */
    EVENT_GROUP_UI_SHELL_FINDME,            /**< group for find me events */
    EVENT_GROUP_UI_SHELL_BT_FAST_PAIR,      /**< group for bt fast pair events */
    EVENT_GROUP_UI_SHELL_AWS_DATA,          /**< group for BT AWS report events */
    EVENT_GROUP_UI_SHELL_LED_MANAGER,       /**< group for LED manager events */
    EVENT_GROUP_UI_SHELL_AUDIO_ANC,         /**< group for audio events */
    EVENT_GROUP_UI_SHELL_POWER_SAVING,      /**< group for power saving app internal events */
    EVENT_GROUP_UI_SHELL_SYSTEM_POWER,      /**< group for system poweroff in OFF/RTC/SLEEP mode events */
    EVENT_GROUP_UI_SHELL_BT_AMA,            /**< group for bt AMA events */
    EVENT_GROUP_UI_SHELL_MULTI_VA,          /**< group for multi voice assistant */
    EVENT_GROUP_UI_SHELL_VA_XIAOWEI,        /**< group for VA - xiaowei */
    EVENT_GROUP_UI_SHELL_GSOUND,            /**< group for GSound events */
    EVENT_GROUP_UI_SHELL_XIAOAI,            /**< group for Xiaoai events */
    EVENT_GROUP_UI_SHELL_LE_AUDIO,          /**< group for LE Audio events */
    EVENT_GROUP_UI_SHELL_TOUCH_KEY,          /**< group for touch key events */
    EVENT_GROUP_UI_SHELL_ANC_FF_RACE,       /**< group for adaptive ANC events sent from race module. */
    EVENT_GROUP_UI_SHELL_ANC_FF_AUDIO,      /**< group for adaptive ANC events sent from audio module. */
    EVENT_GROUP_UI_SHELL_MS_TEAMS = 30,     /**< group for MS teams. */
    EVENT_GROUP_SWITCH_AUDIO_PATH,          /**< group for line in audio path switch */
    EVENT_GROUP_SWITCH_USB_AUDIO,           /**< group for usb audio */
    EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN,   /**< group for adaptive ANC extend gain. */
    EVENT_GROUP_UI_SHELL_DUAL_CHIP_CMD,     /**< group for special message between dual chip mode race cmd */
    EVENT_GROUP_UI_SHELL_USB_AUDIO,           /**< group for USB driver callback */
    EVENT_GROUP_UI_SHELL_AMI_VENDOR,        /**< group for callback registered in ami_register_vendor_se(). */
    EVENT_GROUP_UI_SHELL_DONGLE_DATA,        /**< group for ULL report events */
    EVENT_GROUP_UI_SHELL_XOBX = 40,         /**< group for XBOX events */
    EVENT_GROUP_UI_SHELL_APP_SERVICE = 41,  /**< group for app service */
    EVENT_GROUP_UI_SHELL_ADC_KEY,           /**< group for ADC key events */
} apps_event_group_t;

#endif /* __APPS_EVENTS_EVENT_GROUP_H__ */
