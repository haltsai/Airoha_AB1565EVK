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

#ifndef _SCENARIO_ULL_AUDIO_H_
#define _SCENARIO_ULL_AUDIO_H_

#if defined(AIR_GAMING_MODE_DONGLE_ENABLE)

/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "types.h"
#include "source_inter.h"
#include "sink_inter.h"
#include "source.h"
#include "sink.h"
#include "transform_inter.h"
#include "common.h"
#include "stream_audio_hardware.h"
#include "stream_audio_setting.h"
#include "stream_audio_driver.h"
#include "dsp_audio_msg_define.h"
#include "dsp_audio_msg.h"
#include "hal_hw_semaphore.h"
#include "hal_resource_assignment.h"
#ifdef AIR_SOFTWARE_CLK_SKEW_ENABLE
#include "clk_skew_sw.h"
#endif /* AIR_SOFTWARE_CLK_SKEW_ENABLE */
#ifdef AIR_SOFTWARE_SRC_ENABLE
#include "sw_src_interface.h"
#endif /* AIR_SOFTWARE_SRC_ENABLE */
#ifdef AIR_CELT_ENC_ENABLE
#include "celt_enc_interface.h"
#endif /* AIR_CELT_ENC_ENABLE */
#ifdef AIR_SOFTWARE_GAIN_ENABLE
#include "sw_gain_interface.h"
#endif /* AIR_SOFTWARE_GAIN_ENABLE */
#ifdef AIR_SOFTWARE_BUFFER_ENABLE
#include "sw_buffer_interface.h"
#endif /* AIR_SOFTWARE_BUFFER_ENABLE */
#ifdef AIR_SOFTWARE_MIXER_ENABLE
#include "sw_mixer_interface.h"
#endif /* AIR_SOFTWARE_MIXER_ENABLE */

/* Public define -------------------------------------------------------------*/


/* Public typedef ------------------------------------------------------------*/
typedef enum {
    GAMING_MODE_STREAM_DATA_EMPTY,
    GAMING_MODE_STREAM_DATA_IN_STREAM,
    GAMING_MODE_STREAM_DATA_IN_MIXER,
} gaming_mode_stream_data_status_t;

typedef enum {
    GAMING_MODE_STREAM_MIXER_UNMIX = 0,
    GAMING_MODE_STREAM_MIXER_MIX,
} gaming_mode_stream_mixer_status_t;

typedef enum {
    GAMING_MODE_COMPENSATORY_METHOD_DISABLE = 0,
    GAMING_MODE_COMPENSATORY_METHOD_CAPID_ADJUST,
    GAMING_MODE_COMPENSATORY_METHOD_SW_CLK_SKEW,
} gaming_mode_compensatory_method_t;

typedef struct gaming_mode_dongle_dl_handle_t gaming_mode_dongle_dl_handle_t;
struct gaming_mode_dongle_dl_handle_t {
    void *owner;
    int16_t total_number;
    int16_t index;
    gaming_mode_stream_data_status_t data_status;
    uint32_t process_frames;
    uint32_t read_offset;
    gaming_mode_dongle_dl_handle_t *next_dl_handle;
    uint32_t data_in_gpt_count;
#ifdef AIR_SOFTWARE_GAIN_ENABLE
    sw_gain_port_t *gain_port;
#endif /* AIR_SOFTWARE_GAIN_ENABLE */
#ifdef AIR_SOFTWARE_CLK_SKEW_ENABLE
    sw_clk_skew_port_t *clk_skew_port;
    gaming_mode_compensatory_method_t compen_method;
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
    gaming_mode_stream_mixer_status_t mixer_status;
    sw_mixer_member_t *mixer_member;
#endif /* AIR_SOFTWARE_MIXER_ENABLE */
};

typedef enum {
    GAMING_MODE_UL_STREAM_DEINIT,
    GAMING_MODE_UL_STREAM_INIT,
    GAMING_MODE_UL_STREAM_START,
    GAMING_MODE_UL_STREAM_RUNNING,
} gaming_mode_dongle_ul_stream_status_t;

typedef enum {
    GAMING_MODE_DONGLE_UL_FIRST_PACKET_NOT_READY,
    GAMING_MODE_DONGLE_UL_FIRST_PACKET_READY,
    GAMING_MODE_DONGLE_UL_FIRST_PACKET_TIMEOUT,
} gaming_mode_dongle_ul_first_packet_status_t;

typedef enum {
    GAMING_MODE_DONGLE_UL_DATA_EMPTY,
    GAMING_MODE_DONGLE_UL_DATA_NORMAL,
    GAMING_MODE_DONGLE_UL_DATA_PLC,
    GAMING_MODE_DONGLE_UL_DATA_BYPASS_DECODER,
} gaming_mode_dongle_ul_data_status_t;

typedef struct gaming_mode_dongle_ul_handle_t gaming_mode_dongle_ul_handle_t;
struct gaming_mode_dongle_ul_handle_t {
    void *owner;
    int16_t total_number;
    int16_t index;
    uint32_t process_frames;
    uint32_t latency_us;
    uint32_t bt_clk;
    uint16_t play_en_status;
    uint16_t play_en_overflow;
    uint32_t first_packet_bt_clk;
    uint32_t first_anchor_bt_clk;
    uint32_t play_en_bt_clk;
    uint8_t fetch_flag;
    uint16_t frame_count;
    uint16_t frame_index;
    gaming_mode_dongle_ul_stream_status_t stream_status;
    gaming_mode_dongle_ul_data_status_t data_status;
    gaming_mode_dongle_ul_handle_t *next_ul_handle;
    uint32_t data_in_gpt_count;
#ifdef AIR_SOFTWARE_SRC_ENABLE
    sw_src_port_t *src_port;
#endif /* AIR_SOFTWARE_SRC_ENABLE */
#ifdef AIR_SOFTWARE_GAIN_ENABLE
    sw_gain_port_t *gain_port;
#endif /* AIR_SOFTWARE_GAIN_ENABLE */
#ifdef AIR_SOFTWARE_CLK_SKEW_ENABLE
    sw_clk_skew_port_t *clk_skew_port;
    int16_t compen_samples;
    int16_t clk_skew_count;
    int32_t predicted_compen_times;
    int32_t predicted_compen_count;
    sw_clk_skew_compensation_mode_t compensation_mode;
#endif /* AIR_SOFTWARE_CLK_SKEW_ENABLE */
#ifdef AIR_SOFTWARE_BUFFER_ENABLE
    sw_buffer_port_t *buffer_port;
    uint32_t buffer_watermark;
    uint32_t buffer_output_size;
#ifdef AIR_GAMING_MODE_DONGLE_ECNR_ENABLE
    sw_buffer_port_t *ecnr_in_buffer_port;
    sw_buffer_port_t *ecnr_out_buffer_port;
    sw_buffer_list_t *ecnr_buffer_list;
#endif /* AIR_GAMING_MODE_DONGLE_ECNR_ENABLE */
#endif /* AIR_SOFTWARE_BUFFER_ENABLE */
};

typedef struct {
    uint32_t crc32;
    uint8_t seq_num;
    uint8_t frame_size;
    uint16_t check_sum;
} bt_common_gaming_mode_music_frame_header_t;

typedef struct {
    uint8_t frame_num;
    uint8_t frame_size;
    uint16_t check_sum;
    uint32_t anchor;
} bt_common_gaming_mode_voice_frame_header_t;

/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
extern gaming_mode_dongle_dl_handle_t *gaming_mode_dongle_get_dl_handle(void *owner);
extern void gaming_mode_dongle_release_dl_handle(gaming_mode_dongle_dl_handle_t *handle);
extern gaming_mode_dongle_ul_handle_t *gaming_mode_dongle_get_ul_handle(void *owner);
extern void gaming_mode_dongle_release_ul_handle(gaming_mode_dongle_ul_handle_t *handle);
extern void gaming_mode_dongle_mixer_postcallback(sw_mixer_member_t *member, void *para, uint32_t *out_frame_size);
extern void gaming_mode_dongle_dl_init(SOURCE source, audio_transmitter_open_param_p open_param);
extern void gaming_mode_dongle_dl_deinit(SOURCE source, SINK sink);
extern void gaming_mode_dongle_dl_ccni_handler(hal_ccni_event_t event, void * msg);
extern bool gaming_mode_dongle_dl_config(SOURCE source, stream_config_type type, U32 value);
extern bool gaming_mode_dongle_dl_source_get_avail_size(SOURCE source, uint32_t *avail_size);
extern uint32_t gaming_mode_dongle_dl_source_copy_payload(SOURCE source, uint8_t *src_buf, uint8_t *dst_buf, uint32_t length);
extern uint32_t gaming_mode_dongle_dl_source_get_new_read_offset(SOURCE source, U32 amount);
extern void gaming_mode_dongle_dl_source_drop_postprocess(SOURCE source, uint32_t amount);
extern bool gaming_mode_dongle_dl_source_close(SOURCE source);
extern bool gaming_mode_dongle_dl_sink_get_avail_size(SINK sink, uint32_t *avail_size);
extern uint32_t gaming_mode_dongle_dl_sink_copy_payload(SINK sink, uint8_t *src_buf, uint32_t length);
extern bool gaming_mode_dongle_dl_sink_get_new_write_offset(SINK sink, U32 amount, uint32_t *new_write_offset);
extern bool gaming_mode_dongle_dl_sink_query_notification(SINK sink, bool *notification_flag);
extern bool gaming_mode_dongle_dl_sink_send_data_ready_notification(SINK sink);
extern void gaming_mode_dongle_dl_sink_flush_postprocess(SINK sink, uint32_t amount);
extern bool gaming_mode_dongle_dl_sink_close(SINK sink);
extern void gaming_mode_dongle_ul_init(SOURCE source, SINK sink, audio_transmitter_open_param_p audio_transmitter_open_param, bt_common_open_param_p bt_common_open_param);
extern void gaming_mode_dongle_ul_deinit(SOURCE source, SINK sink);
extern void gaming_mode_dongle_ul_start(SOURCE source);
extern void gaming_mode_dongle_ul_ccni_handler(hal_ccni_event_t event, void * msg);
extern bool gaming_mode_dongle_ul_config(SOURCE source, stream_config_type type, U32 value);
extern bool gaming_mode_dongle_ul_source_get_avail_size(SOURCE source, uint32_t *avail_size);
extern uint32_t gaming_mode_dongle_ul_source_copy_payload(SOURCE source, uint8_t *dst_buf, uint32_t length);
extern bool gaming_mode_dongle_ul_source_get_new_read_offset(SOURCE source, U32 amount, U32 *new_read_offset);
extern void gaming_mode_dongle_ul_source_drop_postprocess(SOURCE source, uint32_t amount);
extern bool gaming_mode_dongle_ul_source_close(SOURCE source);
extern bool gaming_mode_dongle_ul_sink_get_avail_size(SINK sink, uint32_t *avail_size);
extern uint32_t gaming_mode_dongle_ul_sink_copy_payload(SINK sink, uint8_t *src_buf, uint8_t *dst_buf, uint32_t length);
extern bool gaming_mode_dongle_ul_sink_close(SINK sink);
#ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
extern void gaming_mode_dongle_game_chat_volume_smart_balance_enable(void *msg);
extern void gaming_mode_dongle_game_chat_volume_smart_balance_do_process(void *msg);
extern void gaming_mode_dongle_game_chat_volume_smart_balance_disable(void *msg);
#endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */

#endif /* AIR_GAMING_MODE_DONGLE_ENABLE */

#endif /* _SCENARIO_ULL_AUDIO_H_ */
