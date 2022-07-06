
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
 * File: app_service.h
 *
 * Description: This file defines the interface of app_service.c.
 *
 */


#ifndef __APP_DONGLE_SERVICE_H__
#define __APP_DONGLE_SERVICE_H__

#include "ui_shell_activity.h"
#include "stdint.h"

#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The dongle event which should be sent from dongle to headset.
 *
 */
typedef enum {
    APP_DONGLE_SERVICE_DONGLE_EVENT_NONE                       = 0x00,
    APP_DONGLE_SERVICE_DONGLE_EVENT_HANDSHAKE_START_ACK        = 0x01,
    APP_DONGLE_SERVICE_DONGLE_EVENT_HANDSHAKE_DONE_ACK         = 0x02,
    APP_DONGLE_SERVICE_DONGLE_EVENT_MODE_UPDATE                = 0x03,
    APP_DONGLE_SERVICE_DONGLE_EVENT_STATE_OFF                  = 0x10,
    APP_DONGLE_SERVICE_DONGLE_EVENT_STATE_RESET                = 0x11,
    APP_DONGLE_SERVICE_DONGLE_EVENT_VOLUME_STATE_UPDATE        = 0x20,
} app_dongle_service_dongle_event_t;

/**
 * @brief The headset event which should be sent from headset to dongle.
 *
 */
typedef enum {
    APP_DONGLE_SERVICE_HEADSET_EVENT_NONE                      = 0x00,
    APP_DONGLE_SERVICE_HEADSET_EVENT_HANDSHAKE_START           = 0X01,
    APP_DONGLE_SERVICE_HEADSET_EVENT_HADNSHAKE_DONE            = 0x02,
    APP_DONGLE_SERVICE_HEADSET_EVENT_VERSION                   = 0x10,
    APP_DONGLE_SERVICE_HEADSET_EVENT_BATTERY_LEVEL             = 0x11,
    APP_DONGLE_SERVICE_HEADSET_EVENT_CHARGING_STATE            = 0x12,
    APP_DONGLE_SERVICE_HEADSET_EVENT_POWER_OFF                 = 0x20,
} ap_dongle_service_headset_event_t;

typedef enum {
    APP_DONGLE_SERVICE_DONGLE_MODE_NONE                = 0x00,
    APP_DONGLE_SERVICE_DONGLE_MODE_XBOX                = 0x01,
    APP_DONGLE_SERVICE_DONGLE_MODE_PC                  = 0x02,

    APP_DONGLE_SERVICE_DONGLE_MODE_MAX                 = 0xFF,
} app_dongle_service_dongle_mode_t;

#define APP_DONGLE_SERVICE_HEADSET_FW_VERSION_BUF_LEN 4

#define APP_DONGLE_SERVICE_VOLUME_NONE                 0x0000  /** Invalid volume status */
#define APP_DONGLE_SERVICE_SPEAKER_VOLUME_MAX          0x0001  /** Max speaker volume */
#define APP_DONGLE_SERVICE_SPEAKER_VOLUME_MIN          0x0002  /** Min speaker volume */
#define APP_DONGLE_SERVICE_MIC_VOLUME_MAX              0x0004  /** Max Mic volume */
#define APP_DONGLE_SERVICE_MIC_VOLUME_MIN              0x0008  /** Min MIC volume */
#define APP_DONGLE_SERVICE_GAME_CHAT_BALANCE_MAX       0x0010  /** Max gaming chat balance volume, means full chat */
#define APP_DONGLE_SERVICE_GAME_CHAT_BALANCE_MIN       0x0020  /** Min gaming chat balance volume, means full game audio */
#define APP_DONGLE_SERVICE_SIDE_TONE_MAX               0x0040  /** Max side tone volume */
#define APP_DONGLE_SERVICE_SIDE_TONE_MIN               0x0080  /** Min side tone volume */
#define APP_DONGLE_SERVICE_MIC_MUTE                    0x0100  /** Mic mute */
#define APP_DONGLE_SERVICE_SPEAKER_MUTE                0x0200  /** Speaker mute */
#define APP_DONGLE_SERVICE_SIDE_TONE_ENABLE            0x0400  /** Side tone enabled */
#define APP_DONGLE_SERVICE_GAME_CHAT_BALANCE_AVG       0x0800  /** Average gaming/chat balance volume */

#define APP_DONGLE_SERVICE_VOLUME_CHANGE_NONE              0x00
#define APP_DONGLE_SERVICE_VOLUME_CHANGE_MIC               0x01
#define APP_DONGLE_SERVICE_VOLUME_CHANGE_SPEAKER           0x02
#define APP_DONGLE_SERVICE_VOLUME_CHANGE_BALANCE           0x03
#define APP_DONGLE_SERVICE_VOLUME_CHANGE_SIDETONE          0x04
#define APP_DONGLE_SERVICE_VOLUME_CHANGE_MIC_MUTE          0x05
#define APP_DONGLE_SERVICE_VOLUME_CHANGE_SPEAKER_MUTE      0x06
#define APP_DONGLE_SERVICE_VOLUME_CHANGE_SIDETONE_ENABLE   0x07

typedef struct {
    uint32_t            new_volume;
    uint8_t             which_changed;
} __attribute__((packed)) app_srv_volume_change_t;

/**
* @brief      This function is the interface of the app_service_activity, and is only called by ui_shell framework when events are sent.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_group, the current event group to be handled.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
bool app_dongle_service_activity_proc(struct _ui_shell_activity *self,
                                        uint32_t event_group,
                                        uint32_t event_id,
                                        void *extra_data,
                                        size_t data_len);

void app_dongle_service_set_fw_version(uint32_t fw_version);

bool app_dongle_service_is_dongle_connected(void);

app_dongle_service_dongle_mode_t app_dongle_service_get_dongle_mode(void);

typedef struct {
    void (*dongle_connected)();
    void (*dongle_disconnected)();
    void (*dongle_off)();
    void (*dongle_reset)();
    void (*dongle_mode_changed)(app_dongle_service_dongle_mode_t new_mode);
    void (*dongle_volume_status_changed)(app_srv_volume_change_t *new_volume_status);
} app_dongle_service_callback_t;

void app_dongle_service_register_callback(app_dongle_service_callback_t callback_table);

#ifdef __cplusplus
}
#endif

#endif /* AIR_BT_ULTRA_LOW_LATENCY_ENABLE */

#endif /* __APP_DONGLE_SERVICE_H__ */

