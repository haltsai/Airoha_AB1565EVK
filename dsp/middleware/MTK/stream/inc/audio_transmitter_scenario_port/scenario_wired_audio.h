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

#ifndef _SCENARIO_WIRED_AUDIO_H_
#define _SCENARIO_WIRED_AUDIO_H_

#if defined(AIR_WIRED_AUDIO_ENABLE)

/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "types.h"
#include "source_inter.h"
#include "sink_inter.h"
#include "source.h"
#include "sink.h"
#include "transform_inter.h"

#include "stream_audio_hardware.h"
#include "stream_audio_setting.h"
#include "stream_audio_driver.h"
#include "dsp_audio_msg_define.h"
#include "dsp_audio_msg.h"
#include "hal_hw_semaphore.h"
#include "hal_resource_assignment.h"

#include "stream_audio_transmitter.h"
#include "dsp_audio_process.h"

#include "dsp_scenario.h"

#ifdef AIR_SOFTWARE_CLK_SKEW_ENABLE
#include "clk_skew_sw.h"
#endif /* AIR_SOFTWARE_CLK_SKEW_ENABLE */
#ifdef AIR_SOFTWARE_GAIN_ENABLE
#include "sw_gain_interface.h"
#endif /* AIR_SOFTWARE_GAIN_ENABLE */
#ifdef AIR_SOFTWARE_BUFFER_ENABLE
#include "sw_buffer_interface.h"
#endif /* AIR_SOFTWARE_BUFFER_ENABLE */
#ifdef AIR_SOFTWARE_MIXER_ENABLE
#include "sw_mixer_interface.h"
#endif /* AIR_SOFTWARE_MIXER_ENABLE */
#ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
#include "volume_estimator_interface.h"
#include "dtm.h"
#include "preloader_pisplit.h"
#include "audio_nvdm_common.h"
#endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */

typedef enum {
    WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_VOICE_L = 0,
    WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_VOICE_R,
    WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_VOICE_DUL,
    WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_MUSIC_L,
    WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_MUSIC_R,
    WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_MUSIC_DUL,
    WIRED_AUDIO_CONFIG_OP_MUSIC_MIX,
    WIRED_AUDIO_CONFIG_OP_MUSIC_UNMIX,
    WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_LINEIN,
    WIRED_AUDIO_CONFIG_OP_MAX = 0xffffffff
} wired_audio_runtime_config_operation_t;

typedef struct {
    int32_t gain_1;
    int32_t gain_2;
} wired_audio_vol_param_t;

typedef struct {
    wired_audio_runtime_config_operation_t          config_operation;
    wired_audio_vol_param_t    vol_level;
} wired_audio_runtime_config_param_t, *wired_audio_runtime_config_param_p;

typedef enum {
    GAMING_MODE_STREAM_DATA_EMPTY,
    GAMING_MODE_STREAM_DATA_IN_STREAM,
    GAMING_MODE_STREAM_DATA_IN_MIXER,
} wired_audio_stream_data_status_t;

typedef enum {
    GAMING_MODE_STREAM_MIXER_UNMIX = 0,
    GAMING_MODE_STREAM_MIXER_MIX,
} wired_audio_stream_mixer_status_t;

typedef enum {
    WIRED_AUDIO_COMPENSATORY_METHOD_DISABLE = 0,
    WIRED_AUDIO_COMPENSATORY_METHOD_CAPID_ADJUST,
    WIRED_AUDIO_COMPENSATORY_METHOD_SW_CLK_SKEW,
} wired_audio_compensatory_method_t;

typedef struct wired_audio_handle_t wired_audio_handle_t;
struct wired_audio_handle_t {
    void *owner;
    int16_t total_number;
    int16_t index;
    wired_audio_stream_data_status_t data_status;
    uint32_t process_frames;
    wired_audio_handle_t *next_dl_handle;
#ifdef AIR_SOFTWARE_GAIN_ENABLE
    sw_gain_port_t *gain_port;
#endif /* AIR_SOFTWARE_GAIN_ENABLE */
#ifdef AIR_SOFTWARE_CLK_SKEW_ENABLE
    sw_clk_skew_port_t *clk_skew_port;
    wired_audio_compensatory_method_t compen_method;
    int16_t compen_samples;
    int16_t compen_flag;
    int16_t total_compen_samples;
    uint16_t compen_count;
    uint16_t cap_id_default;
    uint16_t cap_id_cur;
    uint16_t cap_id_count;
#endif /* AIR_SOFTWARE_CLK_SKEW_ENABLE */
#ifdef AIR_SOFTWARE_BUFFER_ENABLE
    sw_buffer_port_t *buffer_port;
#endif /* AIR_SOFTWARE_BUFFER_ENABLE */
#ifdef AIR_SOFTWARE_MIXER_ENABLE
    wired_audio_stream_mixer_status_t mixer_status;
    sw_mixer_member_t *mixer_member;
#endif /* AIR_SOFTWARE_MIXER_ENABLE */
};

#ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
typedef struct {
    int16_t enable;
    uint16_t gain_setting_update_done;
    volume_estimator_port_t *port;
    sw_gain_port_t *game_path_gain_port;
    void *nvkey_buf;
    uint32_t data_size;
    void *data_buf;
    int32_t current_db;
    int32_t failure_threshold_db;
    int32_t effective_threshold_db;
    int32_t adjustment_amount_db;
    int32_t up_step_db;
    int32_t down_step_db;
    uint32_t effective_delay_ms;
    uint32_t effective_delay_us_count;
    uint32_t failure_delay_ms;
    uint32_t failure_delay_us_count;
    bool active_flag;
    wired_audio_vol_param_t chat_path_default_vol_level;
    wired_audio_vol_param_t game_path_default_vol_level;
    wired_audio_vol_param_t game_path_balance_vol_level;
} wired_audio_usb_vol_balance_handle_t;

typedef struct {
    uint16_t *data_buf_1_head;
    uint16_t *data_buf_1_tail;
    uint16_t *data_buf_1_cur;
    uint16_t *data_buf_2_head;
    uint16_t *data_buf_2_tail;
    uint16_t *data_buf_2_cur;
} wired_audio_usb_vol_balance_buffer_msg_t;

typedef struct {
    int32_t     enable;
    int32_t     effective_threshold_db;
    uint32_t    effective_delay_ms;
    int32_t     failure_threshold_db;
    uint32_t    failure_delay_ms;
    int32_t     adjustment_amount_db;
    int32_t     up_step_db;
    int32_t     down_step_db;
    DSP_PARA_GAME_CHAT_VOL_STRU chat_vol_nvkey;
} wired_audio_usb_vol_balance_nvkey_t;
#endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */

extern CONNECTION_IF *g_application_ptr_usb_in_0;
extern CONNECTION_IF *g_application_ptr_usb_in_1;
extern int8_t wired_audio_get_usb_audio_start_number(void);
extern void wired_audio_add_usb_audio_start_number(void);
extern void wired_audio_minus_usb_audio_start_number(void);
extern wired_audio_handle_t *wired_audio_get_handle(void *owner);
extern bool wire_audio_source_get_avail_size(SOURCE source, uint32_t *avail_size);
extern uint32_t wire_audio_source_copy_payload(SOURCE source, uint8_t *src_buf, uint8_t *dst_buf, uint32_t length);
extern uint32_t wire_audio_source_get_new_read_offset(SOURCE source, U32 amount);
extern void wire_audio_source_drop_postprocess(SOURCE source, uint32_t amount);
extern void wired_audio_release_handle(wired_audio_handle_t *handle);
extern bool wire_audio_source_close(SOURCE source);
extern void wire_audio_runtime_config(audio_transmitter_scenario_sub_id_wiredaudio_t sub_id, void *config_param, wired_audio_handle_t *handle);
#ifdef AIR_SOFTWARE_MIXER_ENABLE
extern void wired_audio_mixer_postcallback(sw_mixer_member_t *member, void *para, uint32_t *out_frame_size);
#endif
#ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
extern void wired_audio_usb_dl_vol_balance_init(SOURCE source, audio_transmitter_open_param_p open_param);
extern void wired_audio_usb_dl_vol_balance_deinit(SOURCE source, SINK sink);
extern void wired_audio_usb_game_chat_volume_smart_balance_enable(void *msg);
extern void wired_audio_usb_game_chat_volume_smart_balance_do_process(void *msg);
extern void wired_audio_usb_game_chat_volume_smart_balance_disable(void *msg);
#endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */

#endif /* AIR_WIRED_AUDIO_ENABLE */

#endif /* _SCENARIO_WIRED_AUDIO_H_ */
