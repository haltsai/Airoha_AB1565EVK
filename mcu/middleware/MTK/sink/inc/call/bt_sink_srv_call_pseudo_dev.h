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
 
#ifndef __BT_SINK_SRV_CALL_PSEUDO_DEV_H__
#define __BT_SINK_SRV_CALL_PSEUDO_DEV_H__

#include "bt_sink_srv.h"
#include "bt_sink_srv_call_audio.h"
#include "bt_connection_manager_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BT_SINK_SRV_CALL_PSD_NUM 0x03

#define BT_SINK_SRV_CALL_EVENT_CONNECT_LINK_REQ     0x80
#define BT_SINK_SRV_CALL_EVENT_DISCONNECT_LINK_REQ  0x81
#define BT_SINK_SRV_CALL_EVENT_SCO_ACTIVATE_REQ     0x82
#define BT_SINK_SRV_CALL_EVENT_SCO_DEACTIVATE_REQ   0x83
#define BT_SINK_SRV_CALL_EVENT_SUSPEND_REQ          0x84

#define BT_SINK_SRV_CALL_EVENT_CONNECT_LINK_REQ_IND 0x00
#define BT_SINK_SRV_CALL_EVENT_LINK_CONNECTED       0x01
#define BT_SINK_SRV_CALL_EVENT_LINK_DISCONNECTED    0x02
#define BT_SINK_SRV_CALL_EVENT_SCO_CONNECTED        0x03
#define BT_SINK_SRV_CALL_EVENT_SCO_DISCONNECTED     0x04
#define BT_SINK_SRV_CALL_EVENT_SCO_ACTIVATED        0x05
#define BT_SINK_SRV_CALL_EVENT_SCO_DEACTIVATED      0x06
#define BT_SINK_SRV_CALL_EVENT_CALL_START_IND       0x07
#define BT_SINK_SRV_CALL_EVENT_CALL_END_IND         0x08
#define BT_SINK_SRV_CALL_EVENT_RING_IND             0x09
#define BT_SINK_SRV_CALL_EVENT_STOP_RING            0x0A
#ifdef AIR_FEATURE_SINK_AUDIO_SWITCH_SUPPORT
#define BT_SINK_SRV_CALL_EVENT_HF_SWITCH_START      0x0B
#define BT_SINK_SRV_CALL_EVENT_HF_SWITCH_STOP       0x0C
#endif
#define BT_SINK_SRV_CALL_EVENT_PLAY_CODEC_IND       0x50
#define BT_SINK_SRV_CALL_EVENT_STOP_CODEC_IND       0x51
typedef uint8_t bt_sink_srv_call_state_event_t;

#define BT_SINK_SRV_CALL_PSD_EVENT_GET_CALL_STATE   0x01
#define BT_SINK_SRV_CALL_PSD_EVENT_GET_SCO_STATE    0x02
#define BT_SINK_SRV_CALL_PSD_EVENT_REJECT           0x03
#define BT_SINK_SRV_CALL_PSD_EVENT_SUSPEND          0x04
#define BT_SINK_SRV_CALL_PSD_EVENT_ACTIVATE_SCO     0x05
#define BT_SINK_SRV_CALL_PSD_EVENT_DEACTIVATE_SCO   0x06
#define BT_SINK_SRV_CALL_PSD_EVENT_IS_HIGHLIGHT     0x07
#define BT_SINK_SRV_CALL_PSD_EVENT_IS_SCO_ACTIVATED 0x08
#define BT_SINK_SRV_CALL_PSD_EVENT_DEINIT           0xFF
typedef uint8_t bt_sink_srv_call_pseudo_dev_event_t;

typedef void(*bt_sink_srv_call_pseudo_dev_callback)(bt_sink_srv_call_pseudo_dev_event_t event_id, void *device, void *params);

void bt_sink_srv_call_psd_init(void);

void bt_sink_srv_call_psd_deinit(void);

void* bt_sink_srv_call_psd_alloc_device(
    bt_bd_addr_t *addr,
    bt_sink_srv_call_pseudo_dev_callback callback);

void bt_sink_srv_call_psd_free_device(void *device);

void bt_sink_srv_call_psd_set_mic_volume(void *device, bt_sink_srv_call_audio_volume_t volume);

void bt_sink_srv_call_psd_set_speaker_volume(void *device, bt_sink_srv_call_audio_volume_t volume);

bt_sink_srv_call_audio_volume_t bt_sink_srv_call_psd_get_mic_volume(void *device);

bt_sink_srv_call_audio_volume_t bt_sink_srv_call_psd_get_speaker_volume(void *device);

void bt_sink_srv_call_psd_init_speaker_volume(void *device, bt_sink_srv_call_audio_volume_t volume);

void bt_sink_srv_call_psd_state_event_notify(void *device, bt_sink_srv_call_state_event_t event, void *data);

bool bt_sink_srv_call_psd_is_ready(void *device);

bool bt_sink_srv_call_psd_is_ready(void *device);

bool bt_sink_srv_call_psd_is_all_in_steady_state(void);

void bt_sink_srv_call_psd_set_device_id(void *device, bt_bd_addr_t *bd_addr);

void bt_sink_srv_call_psd_set_codec_type(void *device, bt_sink_srv_call_audio_codec_type_t codec);

void bt_sink_srv_call_psd_reset_user_callback(void *device, bt_sink_srv_call_pseudo_dev_callback callback);

bt_sink_srv_call_audio_codec_type_t bt_sink_srv_call_psd_get_codec_type(void *device);

#ifdef __cplusplus
}
#endif

#endif /* __BT_SINK_SRV_CALL_PSEUDO_DEV_H__ */

