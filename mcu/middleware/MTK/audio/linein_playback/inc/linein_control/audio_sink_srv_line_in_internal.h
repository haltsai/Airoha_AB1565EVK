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

#ifndef __AUDIO_SINK_SRV_LINE_IN_INTERNAL_H__
#define __AUDIO_SINK_SRV_LINE_IN_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bt_sink_srv_utils.h"
#include "audio_sink_srv_line_in.h"

#define AUDIO_SINK_SRV_LINE_IN_PSEUDO_FLAG_USEED      (1 << 0)

#if defined(__AFE_HS_DC_CALIBRATION__)
    #define AUDIO_SINK_SRV_LINE_IN_OUTPUT_DEVICE HAL_AUDIO_DEVICE_HANDSET
#else
    #define AUDIO_SINK_SRV_LINE_IN_OUTPUT_DEVICE HAL_AUDIO_DEVICE_HEADSET
#endif
#ifdef MTK_DEVELOPMENT_BOARD_HDK
    #define AUDIO_SINK_SRV_LINE_IN_INPUT_DEVICE HAL_AUDIO_DEVICE_MAIN_MIC
#else
    #define AUDIO_SINK_SRV_LINE_IN_INPUT_DEVICE HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC
#endif

#define AUDIO_SINK_SRV_LINE_IN_VOL_DEF_LEV            (bt_sink_srv_ami_get_lineIN_default_volume_level())

#define AUDIO_SINK_SRV_LINE_IN_MAX_VOL_LEV            (bt_sink_srv_ami_get_lineIN_max_volume_level())
#define AUDIO_SINK_SRV_LINE_IN_MIN_VOL_LEV            (AUD_VOL_OUT_LEVEL0)

typedef enum {
    VOLUME_UP     = 0,
    VOLUME_DOWN   = 1,

    VOLUME_TOTAL
} volume_change_type_t;


typedef struct {
    uint8_t flag;
    audio_src_srv_handle_t *hd;
} audio_sink_srv_line_in_pseudo_handle_t;


typedef struct {
    uint16_t sink_delay;                           /**< The delay time between the receive and the rendering, united by 0.1ms. */
    uint16_t sink_feature;                         /**< Supported features for the sink role. */
    uint16_t source_feature;                       /**< Supported features for the source role. */
} audio_line_in_init_params_t;

void audio_sink_srv_line_in_ami_hdr(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *user);

void audio_sink_srv_line_in_control_init(void);
void audio_sink_srv_line_in_control_deinit(void);

audio_sink_srv_line_in_context_t *audio_sink_srv_line_in_get_context(void);

void audio_sink_srv_line_in_state_machine_handle(audio_sink_srv_line_in_device_t *dev, uint32_t evt_id, void *param);

void audio_sink_srv_line_in_fill_audio_src_callback(audio_src_srv_handle_t *handle);

audio_sink_srv_line_in_device_t *audio_sink_srv_line_in_get_device(void);

void audio_sink_srv_line_in_fill_am_aud_param(bt_sink_srv_am_audio_capability_t  *aud_cap, audio_line_in_codec_capability_t *line_in_cap);

audio_src_srv_handle_t *audio_sink_srv_line_in_alloc_pseudo_handle(void);
void audio_sink_srv_line_in_free_pseudo_handle(audio_src_srv_handle_t *hd);
void audio_sink_srv_line_in_init(void);
void audio_sink_srv_line_in_deinit(void);

int32_t audio_sink_srv_line_in_change_volume(uint8_t type, uint8_t in_out);

void audio_sink_srv_line_in_play(audio_src_srv_handle_t *handle);

void audio_sink_srv_line_in_stop(audio_src_srv_handle_t *handle);

void audio_sink_srv_line_in_suspend(audio_src_srv_handle_t *handle, audio_src_srv_handle_t *int_hd);

void audio_sink_srv_line_in_reject(audio_src_srv_handle_t *handle);

void audio_sink_srv_line_in_exception(audio_src_srv_handle_t *handle, int32_t event, void *param);

void audio_sink_srv_line_in_create_handle(void);

void audio_sink_srv_line_in_destroy_handle(void);

void audio_sink_srv_line_in_drv_play(void *param);

void audio_sink_srv_line_in_drv_stop(void *param);

void audio_sink_srv_line_in_play_handle(audio_src_srv_handle_t *handle);

void audio_sink_srv_line_in_stop_handle(audio_src_srv_handle_t *handle);

void audio_sink_srv_line_in_suspend_handle(audio_src_srv_handle_t *handle, audio_src_srv_handle_t *int_hd);

void audio_sink_srv_line_in_reject_handle(audio_src_srv_handle_t *handle);

void audio_sink_srv_line_in_exception_handle(audio_src_srv_handle_t *handle, int32_t event, void *param);

audio_sink_srv_line_in_status_t audio_sink_srv_line_in_action_handler(audio_sink_srv_line_in_action_t action, void *param);

#ifdef __cplusplus
}
#endif

#endif
