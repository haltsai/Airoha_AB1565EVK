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
 * File: apps_events_bt_event.h
 *
 * Description: This file defines the interface and type of apps_events_bt_event.c.
 *
 */

#ifndef __APPS_EVENTS_BT_EVENT_H__
#define __APPS_EVENTS_BT_EVENT_H__

#include "bt_type.h"
#include "bt_aws_mce.h"
#include "bt_gap_le_service.h"
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
#include "bt_ull_service.h"
#endif
#include "bt_device_manager_power.h"

/** @brief
 * This enum defines the event ID of EVENT_GROUP_UI_SHELL_SYSTEM_POWER.
 */
enum {
    APPS_EVENTS_SYSTEM_POWER_PMUOFF,   /**< event to enter OFF mode */
    APPS_EVENTS_SYSTEM_POWER_RTC,      /**< event to enter RTC mode */
    APPS_EVENTS_SYSTEM_POWER_SLEEP,    /**< event to enter SLEEP mode */
};

/**
 *  @brief This structure defines the extra data format of bt events.
 */
typedef struct _bt_event_data {
    bt_status_t status;     /* Status, refer to bt_status_t */
    void *buffer;           /* Buffer, the format is different for different bt events */
} apps_bt_event_data_t;

/**
 *  @brief This structure defines the fromat of suffix part of extra data of bt sink events.
 */
typedef struct _bt_event_suffix_data {
    bt_aws_mce_role_t aws_role;     /* AWS role. refer to bt_aws_mce_role_t. */
} bt_event_suffix_data_t;

/**
 * @brief      Initialize bt event senders.
 */
void apps_events_bt_event_init(void);

/**
 * @brief      Get the suffix data from extra data of bt sink events.
 * @param[in]  event_params, the point to the data.
 * @param[in]  param_len, the data length.
 * @return     The pointer to the suffix data.
 */
bt_event_suffix_data_t *get_bt_event_suffix_data(void *event_params, size_t param_len);

/**
 * @brief      Initialize app callback of race cmd.
 */
void bt_race_app_event_init();

/**
 * @brief      Get event and status by ui event_id.
 * @param[in]  event_id, the event id.
 * @param[out]  p_evt, the pointer to the event.
 * @param[out]  p_status, the pointer to the status.
 */
void bt_event_get_bt_dm_event_and_status(uint32_t event_id, bt_device_manager_power_event_t *p_evt, bt_device_manager_power_status_t *p_status);
;

/**
 * @brief      The LE service callback which should be register when starting BLE adv.
 * @param[in]  event, the event id.
 * @param[in]  data, the received data.
 */
void apps_bt_events_le_service_callback(bt_gap_le_srv_event_t event, void *data);

#ifdef MTK_AWS_MCE_ENABLE
/**
 * @brief      Initialize aws report data callback.
 */
void app_aws_report_event_init();
#endif

#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
/**
 * @brief      The ultra low latency callback.
 * @param[in]  event, the event id.
 * @param[in]  param, the parameter of the event.
 * @param[in]  param_len, the length of the parameter of the event.
 */
void bt_ulla_callback(bt_ull_event_t event, void *param, uint32_t param_len);
#endif

#endif /* __APPS_EVENTS_BT_EVENT_H__ */
