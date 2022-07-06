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

#ifndef __BT_BLE_CODEC_INTERNAL_H__
#define __BT_BLE_CODEC_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef AIR_BT_CODEC_BLE_ENABLED

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "syslog.h"
#include <string.h>

#include "bt_codec.h"
#include "hal_audio.h"

#ifndef HAL_AUDIO_MODULE_ENABLED
#error "please turn on audio feature option on hal_feature_config.h"
#endif

#if defined(MTK_AVM_DIRECT)
    #include "hal_audio_cm4_dsp_message.h"
    #include "hal_audio_internal.h"
#endif

#define REMAINED_PKT_CNT(w, r)  ((w) > (r) ? (w) - (r) : (w) < (r) ? (uint32_t)0xFFFFFFFF - (r) + (w) + 1 : 0)

#define BT_BLE_TRUE     1
#define BT_BLE_FALSE    0

#define BT_BLE_TX_BITSTREAM_SIZE_PER_PACKET             (60)
#define BT_BLE_TX_PCM_RING_BUFFER_SIZE                  (2560)

#define BT_BLE_RX_BITSTREAM_SIZE_PER_PACKET             (30)
#define BT_BLE_RX_PACKET_NUM                            (1 << 4)
#define BT_BLE_RX_PCM_RING_BUFFER_SIZE                  (1280)

#define BT_BLE_HW_SRAM_SIZE         (180)
#define BT_BLE_HW_SRAM_RX_PKT_CNT   (BT_BLE_HW_SRAM_SIZE / BT_BLE_RX_BITSTREAM_SIZE_PER_PACKET)
#define BT_BLE_PKT_TYPE_IDX_MAX 4

typedef enum {
    BT_BLE_MODE_NONE = 0,
    BT_BLE_MODE_SPEECH,
    BT_BLE_MODE_TX_ONLY,
    BT_BLE_MODE_RX_ONLY,
    BT_BLE_MODE_LOOPBACK_WITH_CODEC,
    BT_BLE_MODE_LOOPBACK_WITHOUT_CODEC
} bt_ble_mode_t;

typedef void (*bt_codec_callback_t)(void *parameter);

typedef struct {
    uint32_t packet_size;
    uint32_t packet_number;
    uint32_t tx_packet_count_per_interrupt;
    uint32_t rx_packet_count_per_interrupt;
} bt_ble_packet_information_t;

typedef struct {
    uint32_t internal_buffer_size;
    void *handle;
} bt_ble_library_information_t;

typedef struct {
    uint32_t base_address;
    volatile uint32_t *p_offset_r;
    volatile uint32_t *p_offset_w;
    volatile uint32_t *p_control;
} bt_ble_shared_memory_information_t;

typedef struct {
    bt_media_handle_t                  handle;
    bt_codec_le_audio_t                codec_info;
    bt_ble_shared_memory_information_t mem_info;
    uint8_t                            *stream_out_data;
    uint8_t                            *stream_in_data;
    uint8_t                            *customer_enhanced_stream_in_data;   /* this is for customer enhancement used*/
    uint8_t                            *mix_stream_out_data;
    uint32_t                           *pkt_tmp_buf;
    uint32_t                           control_reg;
    uint32_t                           isr_time;
    bool                               aws_flag;
} bt_ble_codec_internal_handle_t;

void bt_ble_set_shared_memory_information (uint32_t base_address, volatile uint32_t *p_offset_r, volatile uint32_t *p_offset_w, volatile uint32_t *p_control);
void bt_ble_ut_process (void);

bool bt_ble_codec_query_is_running(void);
hal_audio_sampling_rate_t bt_ble_codec_query_sampling_rate(void);
hal_audio_channel_number_t bt_ble_codec_query_channel_number(void);

//--------------------------------------------
// Open Parameters for AVM
//--------------------------------------------

#endif //AIR_BT_CODEC_BLE_ENABLED
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif  /*__BT_HFP_CODEC_INTERNAL_H__*/
