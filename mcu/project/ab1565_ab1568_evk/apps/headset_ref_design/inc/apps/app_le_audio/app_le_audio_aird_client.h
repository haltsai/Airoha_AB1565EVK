/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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

#ifndef __APP_LE_AUDIO_AIRD_CLIENT_H__
#define __APP_LE_AUDIO_AIRD_CLIENT_H__

#ifdef AIR_LE_AUDIO_ENABLE

#include "app_le_audio_aird_utillity.h"

#define APP_LE_AUDIO_AIRD_CLIENT_EVENT_SRV_DISCOVERY_COMPLETE   1
typedef uint8_t app_le_audio_aird_client_event_t;

#define APP_LE_AUDIO_AIRD_CLIENT_STATE_IDLE                     0
#define APP_LE_AUDIO_AIRD_CLIENT_STATE_SRV_DISCOVERY_COMPLETE   1
#define APP_LE_AUDIO_AIRD_CLIENT_STATE_SET_CCCD_TX              2
#define APP_LE_AUDIO_AIRD_CLIENT_STATE_INFOM_DEVICE_STATUS      3
#define APP_LE_AUDIO_AIRD_CLIENT_STATE_READY                    4
#define APP_LE_AUDIO_AIRD_CLIENT_STATE_MAX                      5
typedef uint8_t app_le_audio_aird_client_state_t;

typedef struct {
    bt_handle_t handle;
    bt_status_t status;
} app_le_audio_aird_client_event_srv_discovery_complete_t;

typedef struct {
    bt_handle_t att_handle_rx;
    bt_handle_t att_handle_tx;
    bt_handle_t att_handle_tx_cccd;
    app_le_audio_aird_client_state_t state;
    app_le_audio_aird_mode_t mode;
    bool mic_mute;
    uint8_t vol;
    uint8_t *action_queue;
} app_le_audio_aird_client_info_t;

typedef void (*app_le_audio_aird_client_callback_t)(app_le_audio_aird_client_event_t event, void *buff);

bt_status_t app_le_audio_aird_client_register_callback(app_le_audio_aird_client_callback_t callback);

void app_le_audio_aird_client_init(void);

void app_le_audio_aird_client_event_handler(bt_msg_type_t msg, bt_status_t status, void *buff);

void app_le_audio_aird_client_reset_info(uint8_t link_idx);

app_le_audio_aird_client_info_t *app_le_audio_aird_client_get_info(bt_handle_t handle);

bool app_le_audio_aird_client_is_cis_streaming(bt_handle_t *acl_handle, app_le_audio_aird_mode_t *mode);

bt_status_t app_le_audio_aird_client_send_action(bt_handle_t handle, app_le_audio_aird_action_t action, void *param, uint32_t param_len);

void app_le_audio_aird_client_notify_dongle_media_state(bool suspend, bt_handle_t handle, uint8_t *param, uint32_t param_len);

#endif /* AIR_LE_AUDIO_ENABLE */
#endif /* __APP_LE_AUDIO_AIRD_CLIENT_H__ */

