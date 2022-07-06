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
 * File: apps_state_report.h
 *
 * Description: This file defines the interface of apps_state_report.c.
 *
 */

#ifndef __APP_STATE_REPORT__
#define __APP_STATE_REPORT__

#include <stdbool.h>
#include <stdint.h>
#include "ui_shell_activity.h"

/**
 *  @brief This enum defines the bt_app report type.
 */
typedef enum {
    BT_APP_REPORT_TYPE_NONE,                    /**<  None. */
    BT_APP_REPORT_TYPE_STATE,                   /**<  Report BT_SINK_SRV state. */
    BT_APP_REPORT_TYPE_CALLER,                  /**<  Report BT HFP incoming call. */
    BT_APP_REPORT_TYPE_MISSED_CALL,             /**<  Report BT HFP missed call. */
    BT_APP_REPORT_TYPE_LINK_LOST,               /**<  Report BT all profile disconnection. */
    BT_APP_REPORT_TYPE_VISIBILITY,              /**<  Report BT visibility. */
    BT_APP_REPORT_TYPE_AWS_MCE_STATE,           /**<  Report BT AWS state. */
    BT_APP_REPORT_TYPE_AWS_ROLE_HANDOVER_CNF,   /**<  Report BT RHO result. */
    BT_APP_REPORT_TYPE_PROFILE_CONN_STATE,      /**<  Report BT Profile connection state. */
    BT_APP_REPORT_TYPE_HF_SCO_STATE,            /**<  Report BT SCO connection state. */
    BT_APP_REPORT_TYPE_MODE_CHANGED,            /**<  Report BT mode changed. */
    BT_APP_REPORT_TYPE_NOTI_NEW,                /**<  Report BT notify new notification. */
    BT_APP_REPORT_TYPE_NOTI_MISSED_CALL,        /**<  Report BT notify missed call notification. */
    BT_APP_REPORT_TYPE_NOTI_SMS                 /**<  Report BT notify new message. */
} bt_app_report_type_t;

/**
* @brief      This function could send BT_APP state via AT CMD response.
* @param[in]  type, BT_APP report type.
* @param[in]  params, report parameter.
*/
void bt_app_report_state_atci(bt_app_report_type_t type, uint32_t params);

/**
* @brief      This function is the interface of the app_event_state_report_activity, and is only called by ui_shell when events are sent.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_group, the current event group to be handled.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
bool app_event_state_report_activity_proc(
    ui_shell_activity_t *self,
    uint32_t event_group,
    uint32_t event_id,
    void *extra_data,
    size_t data_len);

#endif /* __APP_STATE_REPORT__ */

