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
 * File: multi_va_event_id.h
 *
 * Description: This file defines the types of multi_va module.
 *
 */

#ifndef __MULTI_VA_EVENT_ID_H__
#define __MULTI_VA_EVENT_ID_H__

#include "multi_ble_adv_manager.h"

/**
 *  @brief This enum defines event id of EVENT_GROUP_UI_SHELL_MULTI_VA.
 */
typedef enum {
    MULTI_VA_EVENT_SYNC_BLE_ADDRESS,        /**< The event id of sync BLE random address from agent to partner. */
    MULTI_VA_EVENT_SYNC_BLE_CONN_STATE,     /**< The event id of sync BLE connection status from agent to partner. */
    MULTI_VA_EVENT_SYNC_VA_TYPE_TO_PARTNER, /**< The event id of sync voice assistant type from agent to partner. */
    MULTI_VA_EVENT_NOTIFY_BLE_ADDR_CHANGED, /**< The event id of notifying the BLE random address changed to other APPs. */
    MULTI_VA_EVENT_SYNC_REBOOT,             /**< The event id of sync voice assistant type from agent to partner. */
    MULTI_VA_EVENT_SET_VA,                  /**< The event id of received voice assistent setting from smart phone. */
    MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL, /**< The event id of changing the adv interval. */
    MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL_END = MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL + MULTI_ADV_INSTANCE_MAX_COUNT - 1, /**< Because support multi BLE instance, one event id to match one instance. */
    MULTI_VA_EVENT_CHANGE_BLE_ADV_TYPE,     /**< The event id of changing the adv type when add multi adv in one instance. */
    MULTI_VA_EVENT_CHANGE_BLE_ADV_TYPE_END = MULTI_VA_EVENT_CHANGE_BLE_ADV_TYPE + MULTI_ADV_INSTANCE_MAX_COUNT - 1, /**< Because support multi BLE instance, one event id to match one instance. */
} multi_va_event_id_t;

#endif /* __MULTI_VA_EVENT_ID_H__ */