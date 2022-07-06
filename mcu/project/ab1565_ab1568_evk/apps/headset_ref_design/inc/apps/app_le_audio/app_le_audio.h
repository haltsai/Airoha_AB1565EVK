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

#ifndef __APP_LE_AUDIO_H__
#define __APP_LE_AUDIO_H__

#ifdef AIR_LE_AUDIO_ENABLE
#include "ui_shell_activity.h"
#include "bt_hci.h"

/* The LE Audio visibility type.
   0: Always visible.
   1: Only after EDR connected.
   2: Use app_bt_conn_Manager to control LEA ADV (Rule: Start LEA ADV when LEA could be connected by Phone)
 */
#if defined(AIR_LE_AUDIO_ENABLE) && (defined(AIR_MULTI_POINT_ENABLE) || defined(AIR_LE_AUDIO_MULTIPOINT_ENABLE))
#define APP_LE_AUDIO_VISIBILITY_TYPE    (2)
#else
#define APP_LE_AUDIO_VISIBILITY_TYPE    (0)
#endif

#define APP_LE_AUDIO_MAX_LINK_NUM       (2)

#define APP_LE_AUDIO_ADV_TIME           (2 * 60 * 1000)

enum {
    EVENT_ID_LE_AUDIO_ADV_TIMER = 0,                    /**<  LE AUDIO ADV Timer ID. */
    EVENT_ID_LE_AUDIO_START_ADV,                        /**<  LE AUDIO Start ADV ID. */
    EVENT_ID_LE_AUDIO_STOP_ADV,                         /**<  LE AUDIO Stop ADV ID. */
    EVENT_ID_LE_AUDIO_RESTART_ADV,                      /**<  LE AUDIO restart ADV ID. */
    EVENT_ID_LE_AUDIO_CIS_SYNC_STATE,                   /**<  LE AUDIO CIS sync state. */

    EVENT_ID_LE_AUDIO_BIS_START_STREAMING,              /**<  LE AUDIO BIS start streaming. */
    EVENT_ID_LE_AUDIO_BIS_STOP_STREAMING,               /**<  LE AUDIO BIS stop streaming. */
    EVENT_ID_LE_AUDIO_BIS_SYNC_SRC_ADDR,                /**<  LE AUDIO BIS sync SRC addr. */
    EVENT_ID_LE_AUDIO_BIS_SCAN_TIMEOUT,                 /**<  LE AUDIO BIS scan timeout. */
    EVENT_ID_LE_AUDIO_BIS_SCAN_STOPPED,                 /**<  LE AUDIO BIS scan stopped. */
    EVENT_ID_LE_AUDIO_BIS_SYNC_TO_PEER,                 /**<  LE AUDIO BIS sync info to peer. */
    EVENT_ID_LE_AUDIO_BIS_SYNC_FEATURE,                 /**<  LE AUDIO BIS sync feature. */
    EVENT_ID_LE_AUDIO_BIS_ERROR,                        /**<  LE AUDIO BIS error event. */
    EVENT_ID_LE_AUDIO_NOTIFY_BATTERY                    /**<  LE AUDIO Notify battery via HFP AT CMD. */
};

typedef struct {
    uint32_t timeout;
    bool visible;
} app_ble_audio_adv_aws_data_t;

bool app_le_audio_idle_activity_proc(struct _ui_shell_activity *self,
                                        uint32_t event_group,
                                        uint32_t event_id,
                                        void *extra_data,
                                        size_t data_len);
void app_le_audio_init(void);
void app_le_audio_start_advertising(uint32_t timeout);
void app_le_audio_stop_advertising(void);
void app_le_audio_enable(bool enable);
uint8_t app_le_audio_get_feature_state(void);
void app_le_audio_disconnect(bt_hci_disconnect_reason_t reason);

void app_le_audio_restart_advertising(uint32_t timeout);

uint8_t app_le_audio_get_link_index(bt_handle_t handle);

bt_handle_t app_le_audio_get_handle(uint8_t link_idx);

bool app_le_audio_is_connected(void);

#endif /* AIR_LE_AUDIO_ENABLE */
#endif /* __APP_LE_AUDIO_H__ */

