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
 * File: apps_dongle_sync_event.h
 *
 * Description: This file defines the interface of apps_ull_sync_event.c.
 *
 */

#ifndef __APPS_DONGLE_SYNC_EVENT_H__
#define __APPS_DONGLE_SYNC_EVENT_H__
#include "ui_shell_activity.h"
#include "bt_type.h"

typedef enum {
    APPS_DONGLE_DONGLE,
    APPS_DONGLE_HEADSET,
    APPS_DONGLE_EARBUDS,
} apps_dongle_role_t;

typedef struct {
    uint32_t event_group;
    uint32_t event_id;
    uint32_t extra_data_len;
    uint8_t data[0];
} apps_dongle_event_sync_info_t;

/**
* @brief      This function is used to check the dongle connection state.
* @return     The dongle role.
*/
apps_dongle_role_t apps_get_dongle_role(void);


/**
* @brief      This function is used to check the dongle connection state.
* @return
* #true, the dongle is connected.
* #false, the dongle is not connected.
*/
bool apps_is_dongle_connected(void);

/**
* @brief      This function is used to send UI Shell event with extra_data.
* @param[in]  event_group, the current event group to be sent.
* @param[in]  event_id, the current event ID to be sent.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  extra_data_len, the length of the extra data. 0 means extra_data is NULL.
* @return
* #BT_STATUS_SUCCESS, if the operation is successful.
* #BT_STATUS_FAIL, if the event is not sent successfully.
*/
bt_status_t apps_dongle_sync_event_send_extra(uint32_t event_group,
                                           uint32_t event_id, void *extra_data,
                                           uint32_t extra_data_len);

/**
* @brief      This function is used to send UI Shell event without extra_data .
* @param[in]  event_group, the current event group to be sent.
* @param[in]  event_id, the current event ID to be sent.
* @return
* #BT_STATUS_SUCCESS, if the operation is successful.
* #BT_STATUS_FAIL, if the event is not sent successfully.
*/
bt_status_t apps_dongle_sync_event_send(uint32_t event_group, uint32_t event_id);

/**
* @brief      This function is used to init the dongle event sync.
* @return     None
*/
void apps_dongle_sync_init(void);
#endif

