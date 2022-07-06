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

#ifndef __APP_LE_AUDIO_AIRD_UTILLITY_H__
#define __APP_LE_AUDIO_AIRD_UTILLITY_H__

#ifdef AIR_LE_AUDIO_ENABLE

#include "bt_type.h"

/* AIRD action */
#define APP_LE_AUDIO_AIRD_ACTION_SET_STREAMING_VOLUME_UP    0
#define APP_LE_AUDIO_AIRD_ACTION_SET_STREAMING_VOLUME_DOWN  1
#define APP_LE_AUDIO_AIRD_ACTION_SET_DEVICE_BUSY            2
#define APP_LE_AUDIO_AIRD_ACTION_RESET_DEVICE_BUSY          3
#define APP_LE_AUDIO_AIRD_ACTION_MUTE_MIC                   4
#define APP_LE_AUDIO_AIRD_ACTION_TOGGLE_PLAY                5
#define APP_LE_AUDIO_AIRD_ACTION_BLOCK_STREAM               6
#define APP_LE_AUDIO_AIRD_ACTION_MAX                        7
typedef uint8_t app_le_audio_aird_action_t;

#define APP_LE_AUDIO_AIRD_STREAMING_INTERFACE_UNKNOWN       0x00     /**< Unknown streaming. */
#define APP_LE_AUDIO_AIRD_STREAMING_INTERFACE_SPEAKER       0x01     /**< Streaming speaker. */
#define APP_LE_AUDIO_AIRD_STREAMING_INTERFACE_MICROPHONE    0x02     /**< Streaming microphone. */
typedef uint8_t app_le_audio_aird_streaming_interface_t;

#define APP_LE_AUDIO_AIRD_STREAMING_PORT_0      0x00
#define APP_LE_AUDIO_AIRD_STREAMING_PORT_1      0x01
typedef uint8_t app_le_audio_aird_streaming_port_t;

#define APP_LE_AUDIO_AIRD_CHANNEL_DUAL                           0x00     /**< Dual channel. */
#define APP_LE_AUDIO_AIRD_CHANNEL_LEFT                           0x01     /**< Left channel. */
#define APP_LE_AUDIO_AIRD_CHANNEL_RIGHT                          0x02     /**< Right channel. */
typedef uint8_t app_le_audio_aird_channel_t;

#define APP_LE_AUDIO_AIRD_BLOCK_STREAM_NONE             0x00
#define APP_LE_AUDIO_AIRD_BLOCK_STREAM_UNIDIRECTIONAL   0x01
#define APP_LE_AUDIO_AIRD_BLOCK_STREAM_ALL              0x02
typedef uint8_t app_le_audio_aird_block_stream_t;

/* AIRD event */
#define APP_LE_AUDIO_AIRD_EVENT_MODE_INFO       0x00
#define APP_LE_AUDIO_AIRD_EVENT_MIC_MUTE        0x01
typedef uint8_t app_le_audio_aird_event_t;

/* AIRD mode */
#define APP_LE_AUDIO_AIRD_MODE_NORMOL           0
#define APP_LE_AUDIO_AIRD_MODE_SUPPORT_HID_CALL 1
typedef uint8_t app_le_audio_aird_mode_t;

typedef struct {
    app_le_audio_aird_streaming_interface_t streaming_interface;    /* The streaming type. */
    app_le_audio_aird_streaming_port_t streaming_port;              /* The streaming port number. */
    app_le_audio_aird_channel_t channel;
    uint8_t volume;
} app_le_audio_aird_action_set_streaming_volume_t;

typedef struct {
    app_le_audio_aird_block_stream_t type;
} app_le_audio_aird_action_block_stream_t;

BT_PACKED (
typedef struct {
    app_le_audio_aird_event_t evt;
    app_le_audio_aird_mode_t mode;
}) app_le_audio_aird_event_mode_info_ind_t;

BT_PACKED (
typedef struct {
    app_le_audio_aird_event_t evt;
    bool mic_mute;
}) app_le_audio_aird_event_mic_mute_ind_t;


#endif  /* AIR_LE_AUDIO_ENABLE */
#endif  /* __APP_LE_AUDIO_AIRD_UTILLITY_H__ */


