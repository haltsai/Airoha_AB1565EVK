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

#ifndef _SCENARIO_BLE_AUDIO_H_
#define _SCENARIO_BLE_AUDIO_H_

#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)

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
#include "clk_skew_sw.h"
// #include "sw_src_interface.h"
#include "sw_gain_interface.h"
#include "sw_buffer_interface.h"
#include "sw_mixer_interface.h"
#include "sw_src_interface.h"

/* Public define -------------------------------------------------------------*/
#define BLE_AUDIO_DATA_CHANNEL_NUMBER            2

/* Public typedef ------------------------------------------------------------*/
typedef enum {
    BLE_AUDIO_SOURCE_STREAM_DATA_EMPTY,
    BLE_AUDIO_SOURCE_STREAM_DATA_IN_STREAM,
    BLE_AUDIO_SOURCE_STREAM_DATA_IN_MIXER,
} ble_audio_source_stream_data_status_t;

typedef enum {
    BLE_AUDIO_SOURCE_STREAM_MIXER_UNMIX = 0,
    BLE_AUDIO_SOURCE_STREAM_MIXER_MIX,
} ble_audio_source_stream_mixer_status_t;

typedef enum {
    BLE_AUDIO_SOURCE_COMPENSATORY_METHOD_DISABLE = 0,
    BLE_AUDIO_SOURCE_COMPENSATORY_METHOD_CAPID_ADJUST,
    BLE_AUDIO_SOURCE_COMPENSATORY_METHOD_SW_CLK_SKEW,
} ble_audio_source_compensatory_method_t;

typedef struct  {
    uint8_t  status;
    uint8_t  seq_num;
    uint16_t lc3_packet_frame_size;
    uint16_t lc3_packet_frame_samples;
    uint16_t lc3_packet_frame_interval;
    uint16_t share_buffer_blk_size;
    uint16_t share_buffer_blk_num;
    uint16_t share_buffer_blk_index;
    uint16_t share_buffer_blk_index_previous;
    uint32_t channel_enable;
    SHARE_BUFFER_INFO *p_share_buffer_info[BLE_AUDIO_DATA_CHANNEL_NUMBER];
} LE_AUDIO_INFO;

typedef struct ble_audio_dongle_dl_handle_t ble_audio_dongle_dl_handle_t;
struct ble_audio_dongle_dl_handle_t {
    void *owner;
    int16_t total_number;
    int16_t index;
    ble_audio_dongle_dl_handle_t *next_dl_handle;
    ble_audio_source_stream_data_status_t data_status;
    uint32_t ccni_bt_count;
    uint32_t ccni_gpt_count;
    uint32_t data_in_gpt_count;
    uint32_t bt_out_gpt_count;
    uint32_t process_frames;
    uint16_t usb_frame_size;
    uint16_t usb_channel_num;
    uint16_t usb_frame_samples;
    uint16_t usb_min_start_frames;
    sw_src_port_t *src_port;
    uint16_t src_out_frame_size;
    uint16_t src_out_frame_samples;
    sw_gain_port_t *gain_port;
    sw_clk_skew_port_t *clk_skew_port;
    ble_audio_source_compensatory_method_t compen_method;
    int16_t compen_samples;
    int16_t compen_flag;
    int16_t total_compen_samples;
    uint16_t compen_count;
    uint16_t cap_id_default;
    uint16_t cap_id_cur;
    uint16_t cap_id_count;
    sw_buffer_port_t *buffer_port;
    ble_audio_source_stream_mixer_status_t mixer_status;
    sw_mixer_member_t *mixer_member;
};

typedef enum {
    BLE_AUDIO_DONGLE_UL_STREAM_DEINIT = 0,
    BLE_AUDIO_DONGLE_UL_STREAM_INIT,
    BLE_AUDIO_DONGLE_UL_STREAM_START,
    BLE_AUDIO_DONGLE_UL_STREAM_RUNNING,
} ble_audio_dongle_ul_stream_status_t;

typedef enum {
    BLE_AUDIO_DONGLE_UL_FIRST_PACKET_NOT_READY = 0,
    BLE_AUDIO_DONGLE_UL_FIRST_PACKET_READY,
    BLE_AUDIO_DONGLE_UL_FIRST_PACKET_TIMEOUT,
} ble_audio_dongle_ul_first_packet_status_t;

typedef enum {
    BLE_AUDIO_DONGLE_UL_DATA_EMPTY = 0,
    BLE_AUDIO_DONGLE_UL_DATA_NORMAL,
    BLE_AUDIO_DONGLE_UL_DATA_PLC,
    BLE_AUDIO_DONGLE_UL_DATA_BYPASS_DECODER,
} ble_audio_dongle_ul_data_status_t;

typedef struct  {
    bool enable;
    uint32_t channel_anchor;
    ble_audio_dongle_ul_data_status_t status;
} ble_audio_dongle_ul_channel_info_t;

typedef struct ble_audio_dongle_ul_handle_t ble_audio_dongle_ul_handle_t;
struct ble_audio_dongle_ul_handle_t {
    void *owner;
    int16_t total_number;
    int16_t index;
    ble_audio_dongle_ul_handle_t *next_ul_handle;
    uint8_t fetch_flag;
    uint8_t frame_num;
    uint16_t seq_num;
    uint16_t process_frames;
    uint16_t drop_frames;
    uint16_t play_en_status;
    uint16_t play_en_overflow;
    uint16_t play_en_delay;
    uint16_t play_en_first_packet_safe_delay;
    uint32_t play_en_bt_clk;
    uint32_t first_packet_bt_clk;
    uint32_t first_anchor_bt_clk;
    uint32_t stream_anchor_previous;
    uint32_t stream_anchor;
    uint32_t bt_retry_window;
    uint32_t bt_interval;
    uint32_t bt_channel_anchor_diff;
    uint16_t frame_count;
    uint16_t frame_index;
    ble_audio_dongle_ul_stream_status_t stream_status;
    ble_audio_dongle_ul_channel_info_t channel_info[BLE_AUDIO_DATA_CHANNEL_NUMBER];
    uint16_t usb_frame_size;
    uint16_t usb_channel_num;
    uint16_t usb_frame_samples;
    uint16_t usb_min_start_frames;
    sw_src_port_t *src_port;
    uint16_t src_out_frame_size;
    uint16_t src_out_frame_samples;
    sw_gain_port_t *gain_port;
    sw_clk_skew_port_t *clk_skew_port;
    int16_t compen_samples;
    int16_t clk_skew_count;
    uint32_t clk_skew_watermark_samples;
    sw_buffer_port_t *buffer_port;
    uint32_t buffer_output_size;
    sw_mixer_member_t *mixer_member;
    uint32_t mixer_channel_enable[BLE_AUDIO_DATA_CHANNEL_NUMBER];
};

/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
/******************************************************************************/
/*            BLE audio source dongle music path Public Functions             */
/******************************************************************************/
extern void ble_audio_dongle_mixer_postcallback(sw_mixer_member_t *member, void *para, uint32_t *out_frame_size);
extern void ble_audio_dongle_dl_ccni_handler(hal_ccni_event_t event, void * msg);
extern void ble_audio_dongle_dl_init(SOURCE source, SINK sink, audio_transmitter_open_param_p audio_transmitter_open_param, bt_common_open_param_p bt_common_open_param);
extern void ble_audio_dongle_dl_deinit(SOURCE source, SINK sink);
extern bool ble_audio_dongle_dl_config(SOURCE source, stream_config_type type, U32 value);
extern bool ble_audio_dongle_dl_source_get_avail_size(SOURCE source, uint32_t *avail_size);
extern uint32_t ble_audio_dongle_dl_source_copy_payload(SOURCE source, uint8_t *src_buf, uint8_t *dst_buf, uint32_t length);
extern uint32_t ble_audio_dongle_dl_source_get_new_read_offset(SOURCE source, uint32_t amount);
extern void ble_audio_dongle_dl_source_drop_postprocess(SOURCE source, uint32_t amount);
extern bool ble_audio_dongle_dl_source_close(SOURCE source);
extern bool ble_audio_dongle_dl_sink_get_avail_size(SINK sink, uint32_t *avail_size);
extern uint32_t ble_audio_dongle_dl_sink_copy_payload(SINK sink, uint8_t *src_buf, uint32_t length);
extern bool ble_audio_dongle_dl_sink_get_new_write_offset(SINK sink, U32 amount, uint32_t *new_write_offset);
extern bool ble_audio_dongle_dl_sink_query_notification(SINK sink, bool *notification_flag);
extern bool ble_audio_dongle_dl_sink_send_data_ready_notification(SINK sink);
extern void ble_audio_dongle_dl_sink_flush_postprocess(SINK sink, uint32_t amount);
extern bool ble_audio_dongle_dl_sink_close(SINK sink);
/******************************************************************************/
/*            BLE audio source dongle voice path Public Functions             */
/******************************************************************************/
extern void ble_audio_dongle_ul_init(SOURCE source, SINK sink, audio_transmitter_open_param_p audio_transmitter_open_param, bt_common_open_param_p bt_common_open_param);
extern void ble_audio_dongle_ul_deinit(SOURCE source, SINK sink);
extern void ble_audio_dongle_ul_start(SOURCE source);
extern void ble_audio_dongle_ul_ccni_handler(hal_ccni_event_t event, void * msg);
extern bool ble_audio_dongle_ul_config(SOURCE source, stream_config_type type, U32 value);
extern bool ble_audio_dongle_ul_source_get_avail_size(SOURCE source, uint32_t *avail_size);
extern uint32_t ble_audio_dongle_ul_source_copy_payload(SOURCE source, uint8_t *dst_buf, uint32_t length);
extern bool ble_audio_dongle_ul_source_get_new_read_offset(SOURCE source, U32 amount, U32 *new_read_offset);
extern void ble_audio_dongle_ul_source_drop_postprocess(SOURCE source, uint32_t amount);
extern bool ble_audio_dongle_ul_source_close(SOURCE source);
extern bool ble_audio_dongle_ul_sink_get_avail_size(SINK sink, uint32_t *avail_size);
extern uint32_t ble_audio_dongle_ul_sink_copy_payload(SINK sink, uint8_t *src_buf, uint8_t *dst_buf, uint32_t length);
extern void ble_audio_dongle_ul_sink_query_write_offset(SINK sink, uint32_t *write_offset);
extern bool ble_audio_dongle_ul_sink_close(SINK sink);

#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

#endif /* _SCENARIO_BLE_AUDIO_H_ */
