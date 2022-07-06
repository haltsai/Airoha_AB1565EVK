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
 
#ifndef __BT_SINK_SRV_CALL_PSEUDO_MGR_DEV_H__
#define __BT_SINK_SRV_CALL_PSEUDO_MGR_DEV_H__

#include "bt_sink_srv_call_pseudo_dev.h"
#include "bt_sink_srv_call_audio.h"
#include "audio_src_srv.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BT_SINK_SRV_CALL_PSD_NEXT_STATE_INIT             (0x00)
#define BT_SINK_SRV_CALL_PSD_NEXT_STATE_NONE             (0x01)
#define BT_SINK_SRV_CALL_PSD_NEXT_STATE_CONNECTING       (0x02)
#define BT_SINK_SRV_CALL_PSD_NEXT_STATE_READY            (0x03)
#define BT_SINK_SRV_CALL_PSD_NEXT_STATE_PLAY_NULL        (0x04)
#define BT_SINK_SRV_CALL_PSD_NEXT_STATE_PLAY_RING        (0x05)
#define BT_SINK_SRV_CALL_PSD_NEXT_STATE_PLAY_SCO         (0x06)
#define BT_SINK_SRV_CALL_PSD_NEXT_STATE_ACTIVATE_SCO     (0x07)
#define BT_SINK_SRV_CALL_PSD_NEXT_STATE_DEACTIVATE_SCO   (0x08)
#define BT_SINK_SRV_CALL_PSD_NEXT_STATE_POWER_OFF        (0x09)
typedef uint8_t bt_sink_srv_call_pseudo_dev_next_state_t;

#define BT_SINK_SRV_CALL_PSD_SUB_STATE_NONE             (0x00000000)
#define BT_SINK_SRV_CALL_PSD_SUB_STATE_CONNECTING       (0x00000001)
#define BT_SINK_SRV_CALL_PSD_SUB_STATE_DISCONNECTING    (0x00000003)
#define BT_SINK_SRV_CALL_PSD_SUB_STATE_PLAYING_IDLE     (0x00000100)
#define BT_SINK_SRV_CALL_PSD_SUB_STATE_SCO_ACTIVATING   (0x00000200)
#define BT_SINK_SRV_CALL_PSD_SUB_STATE_CODEC_STARTING   (0x00000300)
#define BT_SINK_SRV_CALL_PSD_SUB_STATE_PLAYING          (0x00000400)
#define BT_SINK_SRV_CALL_PSD_SUB_STATE_CODEC_STOPPING   (0x00000500)
#define BT_SINK_SRV_CALL_PSD_SUB_STATE_SCO_DEACTIVATING (0x00000600)
typedef uint32_t bt_sink_srv_call_pseudo_dev_sub_state_t;

#define BT_SINK_SRV_CALL_PSD_FLAG_NONE             (0x00)
#define BT_SINK_SRV_CALL_PSD_FLAG_ADD_WAIT_LIST    (0x01)
typedef uint8_t bt_sink_srv_call_pseudo_dev_flag_t;

typedef struct {
   void (*play)(audio_src_srv_handle_t *handle);
   void (*stop)(audio_src_srv_handle_t *handle);
   void (*suspend)(audio_src_srv_handle_t *handle, audio_src_srv_handle_t *int_hd);
   void (*reject)(audio_src_srv_handle_t *handle);
   void (*exception_handle)(audio_src_srv_handle_t *handle, int32_t event, void *param);
} bt_sink_srv_call_pseudo_dev_callback_t;

typedef struct {
    audio_src_srv_handle_t *audio_src;
    bt_sink_srv_call_audio_id_t audio_id;
    bt_sink_srv_call_audio_type_t audio_type;
    bt_sink_srv_call_audio_codec_type_t codec;
    bt_sink_srv_call_audio_volume_t mic_vol;
    bt_sink_srv_call_audio_volume_t spk_vol;
    bt_sink_srv_call_pseudo_dev_next_state_t next_state;
    bt_sink_srv_call_pseudo_dev_flag_t flag;
    bt_sink_srv_call_pseudo_dev_callback user_cb;
} bt_sink_srv_call_pseudo_dev_t;

bool bt_sink_srv_call_psd_is_steady(bt_sink_srv_call_pseudo_dev_t *dev);
bool bt_sink_srv_call_psd_is_connecting(bt_sink_srv_call_pseudo_dev_t *dev);
bool bt_sink_srv_call_psd_is_playing(bt_sink_srv_call_pseudo_dev_t *dev);
void bt_sink_srv_call_psd_alloc_audio_src(bt_sink_srv_call_pseudo_dev_t *dev);
void bt_sink_srv_call_psd_free_audio_src(bt_sink_srv_call_pseudo_dev_t *dev);
bt_sink_srv_call_pseudo_dev_t* bt_sink_srv_call_psd_get_device_by_audio_src(audio_src_srv_handle_t *audio_src);
void bt_sink_srv_call_psd_convert_devid_to_btaddr(uint64_t dev_id, bt_bd_addr_t *bd_addr);
uint64_t bt_sink_srv_call_psd_convert_btaddr_to_devid(bt_bd_addr_t *bd_addr);
void bt_sink_srv_call_psd_state_machine( 
        bt_sink_srv_call_pseudo_dev_t *dev,
        bt_sink_srv_call_state_event_t event, 
        void* data);
#ifdef AIR_FEATURE_SINK_AUDIO_SWITCH_SUPPORT
bool bt_sink_srv_call_psd_is_playing_idle(bt_sink_srv_call_pseudo_dev_t *dev);
#endif
bool bt_sink_srv_call_psd_is_playing_codec(bt_sink_srv_call_pseudo_dev_t *dev);

#ifdef __cplusplus
}
#endif
#endif /* __BT_SINK_SRV_CALL_PSEUDO_MGR_DEV_H__ */

