/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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
#ifndef __AUDIO_TRANSMITTER_INTERNAL_H__
#define __AUDIO_TRANSMITTER_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

//#include "bt_sink_srv_ami.h"
#include "FreeRTOS.h"
#include "task.h"
#include "audio_transmitter_control.h"
#include "audio_transmitter_control_port.h"
#include "hal_audio_internal.h"

typedef struct {
    audio_transmitter_state_t state;
    int8_t am_id; //bt_sink_srv_am_id_t
    n9_dsp_share_info_t *p_read_info;
    n9_dsp_share_info_t *p_write_info;
    audio_transmitter_config_t  config;
} audio_transmitter_control_t;

typedef struct {
    audio_transmitter_scenario_type_t scenario_type;
    uint8_t scenario_sub_id;
} audio_transmitter_scenario_list_t;

typedef struct {
    uint16_t sequence_number;
    uint16_t data_length;
} audio_transmitter_block_header_t;

#define AUDIO_TRANSMITTER_MAX 16
extern audio_transmitter_control_t g_audio_transmitter_control[AUDIO_TRANSMITTER_MAX];

bool audio_transmitter_get_is_running_by_scenario_list(audio_transmitter_scenario_list_t *list, uint8_t list_length);
int8_t audio_transmitter_get_am_id_by_scenario(audio_transmitter_scenario_type_t scenario_type, uint8_t scenario_sub_id);
#ifdef __cplusplus
}
#endif

#endif/*__AUDIO_TRANSMITTER_INTERNAL_H__*/
