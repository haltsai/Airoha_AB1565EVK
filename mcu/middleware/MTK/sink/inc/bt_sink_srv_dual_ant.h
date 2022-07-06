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
#ifndef __BT_SINK_SRV_DUAL_ANT_H__
#define __BT_SINK_SRV_DUAL_ANT_H__

#include "bt_type.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_ami.h"
#include "audio_src_srv.h"

#define BT_SINK_DUAL_ANT_AFH_MAPPING_LENGTH 10

#define BT_SINK_DUAL_ANT_FLAG                           (0x01)
#define BT_SINK_DUAL_ANT_TRANSMITTER_STOPPING           ((BT_SINK_DUAL_ANT_FLAG) << 1)

typedef enum
{
    BT_SINK_DUAL_ANT_TYPE_NONE,
    BT_SINK_DUAL_ANT_TYPE_CALL,
    BT_SINK_DUAL_ANT_TYPE_MUSIC,
    BT_SINK_DUAL_ANT_TYPE_CONTROLLER
} bt_sink_srv_dual_ant_type_t;

typedef enum
{
    BT_SINK_DUAL_ANT_TRANSMITTER_SPK,
    BT_SINK_DUAL_ANT_TRANSMITTER_MIC,
} bt_sink_srv_dual_ant_transmitter_t;

typedef struct
{
    bool esco_state;            /* true: esco connected; false: esco disconnected */
} bt_sink_srv_dual_ant_call_info_t;

typedef struct
{
    bool a2dp_state;            /* true: streaing start; false: streaming stop */
} bt_sink_srv_dual_ant_music_info_t;

typedef struct
{
    uint8_t afh_map[BT_SINK_DUAL_ANT_AFH_MAPPING_LENGTH];        /* all RF 80 channnel state */
} bt_sink_srv_dual_ant_controller_info_t;

typedef struct
{
    bt_sink_srv_dual_ant_type_t type;
    union {
        bt_sink_srv_dual_ant_call_info_t        call_info;
        bt_sink_srv_dual_ant_music_info_t       music_info;
        bt_sink_srv_dual_ant_controller_info_t  controller_info;
    };
} bt_sink_srv_dual_ant_data_t;

typedef struct
{
    audio_transmitter_id_t                  transmit_id;
    bool                                    is_transmitter_start;           /**< transmitter is start or not */
    bool                                    is_request_transmitter_start;   /**< user request transmitter start or stop */
    uint32_t                                streaming_flag;                 /**< streaming flag <record streaming temporary state> */
    audio_src_srv_resource_manager_handle_t *resource_handle;               /**< need to take resource before start transmitter */
} bt_sink_srv_dual_transmit_t;


typedef struct
{
    bt_sink_srv_dual_ant_call_info_t    call_info;
    bt_sink_srv_dual_ant_music_info_t   music_info;
    bt_sink_srv_dual_transmit_t         spk_transmit;
    bt_sink_srv_dual_transmit_t         mic_transmit;
} bt_sink_srv_dual_ant_context_t;

void bt_sink_srv_dual_ant_init(void);
bt_sink_srv_dual_ant_context_t* bt_sink_srv_dual_ant_get_context(void);
bt_status_t bt_sink_srv_dual_ant_notify(bool is_critical, bt_sink_srv_dual_ant_data_t *data);

#endif
