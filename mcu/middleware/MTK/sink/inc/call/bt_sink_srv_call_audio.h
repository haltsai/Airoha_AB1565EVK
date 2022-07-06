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
 
#ifndef __BT_SINK_SRV_CALL_AUDIO_H__
#define __BT_SINK_SRV_CALL_AUDIO_H__

#include "bt_sink_srv_ami.h"
#include "bt_sink_srv_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef bt_sink_srv_am_volume_level_out_t  bt_sink_srv_call_audio_volume_t;
typedef bt_sink_srv_am_id_t bt_sink_srv_call_audio_id_t;
typedef bt_hfp_audio_codec_type_t bt_sink_srv_call_audio_codec_type_t;
typedef bt_sink_srv_am_audio_capability_t bt_sink_srv_call_audio_capability_t;
typedef bt_sink_srv_am_notify_callback bt_sink_srv_call_audio_notify_cb;
typedef bt_sink_srv_am_priority_t bt_sink_srv_call_audio_priority_t;

#define BT_SINK_SRV_CALL_AUDIO_MAX_VOLUME  (AUD_VOL_OUT_MAX - 1)
#define BT_SINK_SRV_CALL_AUDIO_MIN_VOLUME   AUD_VOL_OUT_LEVEL0
#define BT_SINK_SRV_CALL_AUDIO_DEFAULT_VOLUME AUD_VOL_OUT_LEVEL10
#define BT_SINK_SRV_CALL_AUDIO_INVALID_ID  AUD_ID_INVALID

#ifdef MTK_AUDIO_SYNC_ENABLE
#define BT_SINK_SRV_CALL_AUDIO_SYNC_DURATION    0x000186A0U
#define BT_SINK_SRV_CALL_AUDIO_SYNC_TIMEOUT     0xFFFFFFFFU
#endif

typedef enum {
    BT_SINK_SRV_CALL_AUDIO_VOL_ACT_UP,
    BT_SINK_SRV_CALL_AUDIO_VOL_ACT_DOWN
} bt_sink_srv_call_audio_volume_act_t;

typedef enum {
    BT_SINK_SRV_CALL_AUDIO_NONE = 0x00,
    BT_SINK_SRV_CALL_AUDIO_NULL= 0x01,
    BT_SINK_SRV_CALL_AUDIO_SCO = 0x02,
    BT_SINK_SRV_CALL_AUDIO_RING = 0x03,
} bt_sink_srv_call_audio_type_t;

#ifdef MTK_AUDIO_SYNC_ENABLE
typedef struct {
    bt_sink_srv_call_audio_volume_t out_volume;
} bt_sink_srv_call_audio_sync_data_t;
#endif

void bt_sink_srv_call_audio_init(void);
void bt_sink_srv_call_audio_deinit(void);

void bt_sink_srv_call_audio_sco_parameter_init(
        bt_sink_srv_call_audio_capability_t *audio_capability,
        bt_sink_srv_call_audio_codec_type_t codec,
        bt_sink_srv_call_audio_volume_t out_volume);

void bt_sink_srv_call_audio_pcm_parameter_init(
        bt_sink_srv_call_audio_capability_t *audio_capability,
        bt_sink_srv_call_audio_volume_t out_volume);

void bt_sink_srv_call_audio_set_out_volume(bt_sink_srv_call_audio_id_t audio_id, bt_sink_srv_call_audio_volume_t volume);
void bt_sink_srv_call_audio_set_in_volume(bt_sink_srv_call_audio_id_t audio_id, bt_sink_srv_call_audio_volume_t volume);
bt_sink_srv_call_audio_id_t bt_sink_srv_call_audio_codec_open(bt_sink_srv_call_audio_notify_cb callback);
bool bt_sink_srv_call_audio_codec_close(bt_sink_srv_call_audio_id_t audio_id);
bool bt_sink_srv_call_audio_play(
    bt_sink_srv_call_audio_id_t audio_id, bt_sink_srv_call_audio_capability_t *audio_capability);
bool bt_sink_srv_call_audio_stop(bt_sink_srv_call_audio_id_t audio_id);
bool bt_sink_srv_call_audio_continue_play(bt_sink_srv_call_audio_id_t audio_id, 
        void *buffer, uint32_t data_count);
void* bt_sink_srv_call_audio_get_ring(uint32_t *length);
bt_sink_srv_call_audio_volume_t bt_sink_srv_call_audio_volume_local_to_bt(bt_sink_srv_call_audio_volume_t local_volume);
bt_sink_srv_call_audio_volume_t bt_sink_srv_call_audio_volume_bt_to_local(bt_sink_srv_call_audio_volume_t bt_volume);
void bt_sink_srv_call_audio_init_play(uint32_t gap_handle);
void bt_sink_srv_call_audio_side_tone_enable(void);
void bt_sink_srv_call_audio_side_tone_disable(void);
#ifdef MTK_AUDIO_SYNC_ENABLE
void bt_sink_srv_call_audio_sync_callback(bt_sink_srv_sync_status_t status, bt_sink_srv_sync_callback_data_t *data);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BT_SINK_SRV_CALL_AUDIO_H__ */

