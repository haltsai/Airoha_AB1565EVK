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

#include "bt_sink_srv.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_dual_ant.h"
#include "race_cmd_co_sys.h"
#include "bt_gap.h"
#include "avm_external.h"
#include "bt_timer_external.h"
#include "bt_callback_manager.h"

static bt_sink_srv_dual_ant_context_t bt_dual_ant_ctx;

static void bt_sink_srv_dual_ant_callback(bool is_critical, uint8_t *buff, uint32_t len);
static void bt_sink_srv_dual_ant_afh_notification_callback(unsigned char *avoid_map, uint8_t length);
static void bt_sink_srv_dual_ant_start_transmitter(bt_sink_srv_dual_ant_transmitter_t type);
static void bt_sink_srv_dual_ant_stop_transmitter(uint8_t type);
static void bt_sink_srv_dual_ant_spk_transmitter_callback(audio_transmitter_event_t event, void *data, void *user_data);
static void bt_sink_srv_dual_ant_mic_transmitter_callback(audio_transmitter_event_t event, void *data, void *user_data);
static void bt_sink_srv_dual_ant_delay_stop_spk_cb(uint32_t timer_id, uint32_t data);
static bt_status_t bt_sink_srv_dual_ant_gap_event_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);
static void bt_sink_srv_dual_ant_mic_resource_callback(audio_src_srv_resource_manager_handle_t *handle, audio_src_srv_resource_manager_event_t event);

void bt_sink_srv_dual_ant_init()
{
    memset(&bt_dual_ant_ctx, 0x00, sizeof(bt_dual_ant_ctx));
    bt_dual_ant_ctx.spk_transmit.transmit_id = AUD_ID_INVALID;
    bt_dual_ant_ctx.mic_transmit.transmit_id = AUD_ID_INVALID;
    if (false == race_cosys_register_data_callback(RACE_COSYS_MODULE_ID_SINK, bt_sink_srv_dual_ant_callback)) {
        bt_sink_srv_assert(0 && "[dual_ant] race_cosys_register_data_callback fail !!!");
    }
    /* controller require no need change RF/Modem register value */
    /* register bt power on */
    if (BT_STATUS_SUCCESS != bt_callback_manager_register_callback(bt_callback_type_app_event,
        (uint32_t)(MODULE_MASK_SYSTEM), (void *)bt_sink_srv_dual_ant_gap_event_callback)) {
        bt_sink_srv_assert(0 && "[dual_ant] bt_callback_manager_register_callback fail !!!");
    }

    /* register resource manager */
    bt_dual_ant_ctx.mic_transmit.resource_handle = audio_src_srv_resource_manager_construct_handle(
        AUDIO_SRC_SRV_RESOURCE_TYPE_MIC, AUDIO_SRC_SRV_RESOURCE_TYPE_MIC_USER_HFP);
    if (bt_dual_ant_ctx.mic_transmit.resource_handle != NULL) {
        bt_dual_ant_ctx.mic_transmit.resource_handle->callback_func = bt_sink_srv_dual_ant_mic_resource_callback;
        bt_dual_ant_ctx.mic_transmit.resource_handle->priority = AUDIO_SRC_SRV_RESOURCE_TYPE_MIC_USER_HFP_PRIORIRT;
    } else {
        bt_sink_srv_assert(0 && "[dual_ant] audio_src_srv_resource_manager_construct_handle fail !!!");
    }

    bt_pka_dual_ant_register_callback(bt_sink_srv_dual_ant_afh_notification_callback);
    bt_sink_srv_report_id("[sink][dual_ant] init success! ", 0);
}

bt_status_t bt_sink_srv_dual_ant_notify(bool is_critical, bt_sink_srv_dual_ant_data_t *data)
{
    bt_status_t ret = BT_STATUS_FAIL;
    if (race_cosys_send_data(RACE_COSYS_MODULE_ID_SINK, is_critical, (uint8_t*)data, sizeof(bt_sink_srv_dual_ant_data_t))) {
        ret = BT_STATUS_SUCCESS;
    }

    if (BT_SINK_DUAL_ANT_TYPE_CALL == data->type) {
        bt_sink_srv_report_id("[sink][dual_ant] dual_ant_notify, esco_state:0x%x, ret:0x%x", 2, data->call_info.esco_state, ret);
    } else if (BT_SINK_DUAL_ANT_TYPE_MUSIC == data->type) {
        bt_sink_srv_report_id("[sink][dual_ant] dual_ant_notify, a2dp_state:0x%x, ret:0x%x", 2, data->music_info.a2dp_state, ret);
    }
    return ret;
}

void bt_sink_srv_dual_ant_callback(bool is_critical, uint8_t *buff, uint32_t len)
{
    bt_sink_srv_assert(buff && (len == sizeof(bt_sink_srv_dual_ant_data_t)) && "buff or len is error!");
    bt_sink_srv_dual_ant_context_t *ctx = bt_sink_srv_dual_ant_get_context();
    bt_sink_srv_dual_ant_data_t *data = (bt_sink_srv_dual_ant_data_t*)buff;

    if (BT_SINK_DUAL_ANT_TYPE_CALL == data->type) {
        bt_sink_srv_report_id("[sink][dual_ant] dual_ant_callback esco state:0x%x, current state:0x%x", 2, data->call_info.esco_state, ctx->call_info.esco_state);
        if (ctx->call_info.esco_state != data->call_info.esco_state) {
            ctx->call_info.esco_state = data->call_info.esco_state;
            if (data->call_info.esco_state) {
                bt_sink_srv_dual_ant_start_transmitter(BT_SINK_DUAL_ANT_TRANSMITTER_SPK);
                audio_src_srv_resource_manager_take(ctx->mic_transmit.resource_handle);
            } else {
                bt_sink_srv_dual_ant_stop_transmitter(BT_SINK_DUAL_ANT_TRANSMITTER_MIC);
                bt_sink_srv_dual_ant_stop_transmitter(BT_SINK_DUAL_ANT_TRANSMITTER_SPK);
            }
        }
    } else if (BT_SINK_DUAL_ANT_TYPE_MUSIC == data->type) {
        bt_sink_srv_report_id("[sink][dual_ant] dual_ant_callback a2dp state:0x%x, current state:0x%x", 2, data->music_info.a2dp_state, ctx->music_info.a2dp_state);
        if (ctx->music_info.a2dp_state != data->music_info.a2dp_state) {
            ctx->music_info.a2dp_state = data->music_info.a2dp_state;
            if (data->music_info.a2dp_state) {
                bt_sink_srv_dual_ant_start_transmitter(BT_SINK_DUAL_ANT_TRANSMITTER_SPK);
            } else {
                bt_sink_srv_dual_ant_stop_transmitter(BT_SINK_DUAL_ANT_TRANSMITTER_SPK);
            }
        }
    } else if (BT_SINK_DUAL_ANT_TYPE_CONTROLLER == data->type) {
        /* Sync AFH mapping info to controller */
        bt_gap_set_afh_host_channel_classification((bt_hci_cmd_set_afh_host_channel_classification_t *)data->controller_info.afh_map);
    } else {
        bt_sink_srv_assert(0 && "data->type is error !!!");
    }
}

bt_sink_srv_dual_ant_context_t* bt_sink_srv_dual_ant_get_context(void)
{
    return &bt_dual_ant_ctx;
}

void bt_sink_srv_dual_ant_afh_notification_callback(unsigned char *avoid_map, uint8_t length)
{
    //bt_sink_srv_report_id("[sink][dual_ant] afh map:0x%x, len: %d ", 2, avoid_map, length);
    bt_sink_srv_assert(avoid_map && (BT_SINK_DUAL_ANT_AFH_MAPPING_LENGTH == length));

    bt_sink_srv_dual_ant_data_t notify;
    notify.type = BT_SINK_DUAL_ANT_TYPE_CONTROLLER;
    memcpy(notify.controller_info.afh_map, avoid_map, BT_SINK_DUAL_ANT_AFH_MAPPING_LENGTH);
    bt_sink_srv_dual_ant_notify(false, &notify);
}

void bt_sink_srv_dual_ant_start_transmitter(bt_sink_srv_dual_ant_transmitter_t type)
{
    audio_transmitter_config_t config;
    bt_sink_srv_dual_ant_context_t *ctx = bt_sink_srv_dual_ant_get_context();
    bt_sink_srv_dual_transmit_t *p_transmitter = NULL;
    audio_transmitter_status_t status = AUDIO_TRANSMITTER_STATUS_FAIL;

    memset(&config, 0x00, sizeof(config));
    
    if (BT_SINK_DUAL_ANT_TRANSMITTER_SPK == type) {
        config.scenario_type = AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK;
        config.scenario_sub_id = AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK_I2S0_TO_DAC;
        config.msg_handler = bt_sink_srv_dual_ant_spk_transmitter_callback;
        p_transmitter = &(ctx->spk_transmit);
    } else if (BT_SINK_DUAL_ANT_TRANSMITTER_MIC == type) {
    #ifdef AIR_DUAL_CHIP_NR_ON_MASTER_ENABLE
        config.scenario_type = AUDIO_TRANSMITTER_WIRED_AUDIO;
        config.scenario_sub_id = AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_OUT_MASTER;
    #else
        config.scenario_type = AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK;
        config.scenario_sub_id = AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK_ADC_TO_I2S0;
    #endif
        config.msg_handler = bt_sink_srv_dual_ant_mic_transmitter_callback;
        p_transmitter = &(ctx->mic_transmit);
    } else {
        bt_sink_srv_assert(0 && "start_transmitter type known!");
    }

    if (p_transmitter && AUD_ID_INVALID == p_transmitter->transmit_id) {
        p_transmitter->transmit_id = audio_transmitter_init(&config);
    }
    bt_sink_srv_assert(AUD_ID_INVALID != p_transmitter->transmit_id);

    p_transmitter->is_request_transmitter_start = true;
    if (false == p_transmitter->is_transmitter_start) {
        if (BT_SINK_DUAL_ANT_TRANSMITTER_SPK == type) {
            bt_timer_ext_stop(BT_SINK_SRV_DUAL_ANT_SPK_DELAY_STOP_TIMER);
        }
        status = audio_transmitter_start(p_transmitter->transmit_id);
    }

    bt_sink_srv_report_id("[sink][dual_ant] start_transmitter type:0x%x, trans_id:0x%x, result:0x%x", 3, type, p_transmitter->transmit_id, status);
}

void bt_sink_srv_dual_ant_stop_transmitter(uint8_t type)
{
    bt_sink_srv_dual_ant_context_t *ctx = bt_sink_srv_dual_ant_get_context();
    bt_sink_srv_dual_transmit_t *p_transmitter = NULL;
    audio_transmitter_status_t status = AUDIO_TRANSMITTER_STATUS_FAIL;

    if (BT_SINK_DUAL_ANT_TRANSMITTER_SPK == type) {
        p_transmitter = &(ctx->spk_transmit);
    } else if (BT_SINK_DUAL_ANT_TRANSMITTER_MIC == type) {
        p_transmitter = &(ctx->mic_transmit);
    } else {
        bt_sink_srv_assert(0 && "stop_transmitter type known!");
    }
    if (p_transmitter && AUD_ID_INVALID != p_transmitter->transmit_id) {
        p_transmitter->is_request_transmitter_start = false;
        /* delay stop SPK to void frequently enable<->disable */
        if (BT_SINK_DUAL_ANT_TRANSMITTER_SPK == type) {
            bt_timer_ext_start(BT_SINK_SRV_DUAL_ANT_SPK_DELAY_STOP_TIMER, 0, 1500, bt_sink_srv_dual_ant_delay_stop_spk_cb);
        } else {
            if (p_transmitter->is_transmitter_start) {
                am_audio_side_tone_disable();
                status = audio_transmitter_stop(p_transmitter->transmit_id);
            }
        }
    }
    bt_sink_srv_report_id("[sink][dual_ant] stop_transmitter type:0x%x, trans_id:0x%x, result:0x%x", 3, type, p_transmitter->transmit_id, status);
}

void bt_sink_srv_dual_ant_spk_transmitter_callback(audio_transmitter_event_t event, void *data, void *user_data)
{
    bt_sink_srv_report_id("[sink][dual_ant] spk_transmitter_callback event:0x%x", 1, event);

    bt_sink_srv_dual_ant_context_t *ctx = bt_sink_srv_dual_ant_get_context();
    switch (event) {
        case AUDIO_TRANSMITTER_EVENT_START_SUCCESS: {
            ctx->spk_transmit.is_transmitter_start = true;
            bt_sink_srv_report_id("[sink][dual_ant] spk_transmitter_callback,  is_request_transmitter_start = 0x%x", 1, ctx->spk_transmit.is_request_transmitter_start);
            if (false == ctx->spk_transmit.is_request_transmitter_start) {
                if (AUDIO_TRANSMITTER_STATUS_SUCCESS != audio_transmitter_stop(ctx->spk_transmit.transmit_id)) {
                    bt_sink_srv_report_id("[sink][dual_ant][error] spk trans_id:0x%x, audio_transmitter_stop fail", 1, ctx->spk_transmit.transmit_id);
                }
            }
            break;
        }
        case AUDIO_TRANSMITTER_EVENT_START_FAIL:
        case AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS: {
            ctx->spk_transmit.is_transmitter_start = false;
            bt_sink_srv_report_id("[sink][dual_ant] spk_transmitter_callback,  is_request_transmitter_start = 0x%x", 1, ctx->spk_transmit.is_request_transmitter_start);
            if (ctx->spk_transmit.is_request_transmitter_start) {
                audio_transmitter_start(ctx->spk_transmit.transmit_id);
            }
            break;
        }
        default:
            break;
    }
}

void bt_sink_srv_dual_ant_mic_transmitter_callback(audio_transmitter_event_t event, void *data, void *user_data)
{
    bt_sink_srv_report_id("[sink][dual_ant] mic_transmitter_callback event:0x%x", 1, event);

    bt_sink_srv_dual_ant_context_t *ctx = bt_sink_srv_dual_ant_get_context();
    switch (event) {
        case AUDIO_TRANSMITTER_EVENT_START_SUCCESS: {
            ctx->mic_transmit.is_transmitter_start = true;
            bt_sink_srv_report_id("[sink][dual_ant] mic_transmitter_callback,  is_request_transmitter_start = 0x%x", 1, ctx->mic_transmit.is_request_transmitter_start);
            if (false == ctx->mic_transmit.is_request_transmitter_start) {
                if (AUDIO_TRANSMITTER_STATUS_SUCCESS != audio_transmitter_stop(ctx->mic_transmit.transmit_id)) {
                    bt_sink_srv_report_id("[sink][dual_ant][error] mic trans_id:0x%x, audio_transmitter_stop fail", 1, ctx->mic_transmit.transmit_id);
                }
            } else {
                am_audio_side_tone_enable();
            }
            break;
        }
        case AUDIO_TRANSMITTER_EVENT_START_FAIL:
        case AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS: {
            ctx->mic_transmit.is_transmitter_start = false;
            bt_sink_srv_report_id("[sink][dual_ant] mic_transmitter_callback,  is_request_transmitter_start = 0x%x", 1, ctx->mic_transmit.is_request_transmitter_start);
            if (ctx->mic_transmit.is_request_transmitter_start) {
                audio_transmitter_start(ctx->mic_transmit.transmit_id);
            } else {
                audio_src_srv_resource_manager_give(ctx->mic_transmit.resource_handle);
            }
            break;
        }
        default:
            break;
    }
}

void bt_sink_srv_dual_ant_delay_stop_spk_cb(uint32_t timer_id, uint32_t data)
{
    bt_sink_srv_dual_ant_context_t *ctx = bt_sink_srv_dual_ant_get_context();
    bt_sink_srv_dual_transmit_t *p_transmitter = NULL;
    audio_transmitter_status_t status = AUDIO_TRANSMITTER_STATUS_FAIL;

    p_transmitter = &(ctx->spk_transmit);
    if (p_transmitter && AUD_ID_INVALID != p_transmitter->transmit_id
) {
        if (false == p_transmitter->is_request_transmitter_start) {
            if (p_transmitter->is_transmitter_start) {
                status = audio_transmitter_stop(p_transmitter->transmit_id);
            }
        }
    }
    bt_sink_srv_report_id("[sink][dual_ant] delay_stop_spk_cb is_request_transmitter_start:0x%x, trans_id:0x%x, result:0x%x", 3, p_transmitter->is_request_transmitter_start, p_transmitter->transmit_id, status);
}

bt_status_t bt_sink_srv_dual_ant_gap_event_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    extern void DRV_2WIRE_Write(uint16_t Addr, uint8_t Data);

    if (BT_POWER_ON_CNF == msg) {
        bt_sink_srv_report_id("[sink][dual_ant] BT Power On", 0);
        /* write RF register for 68 & 65 */
        DRV_2WIRE_Write(0x163, 0x23);
        DRV_2WIRE_Write(0x1A6, 0x9B);
        /* write modem register for 68 & 65 */
        *((volatile unsigned int *)0xB1000010) = 0x00300053;
        *((volatile unsigned int *)0xB1000014) = 0x01F40010;
        *((volatile unsigned int *)0xB1000144) = 0x0097006F;
        *((volatile unsigned int *)0xB1000178) = 0x00460032;
    }
    return BT_STATUS_SUCCESS;
}

static void bt_sink_srv_dual_ant_mic_resource_callback(audio_src_srv_resource_manager_handle_t *handle, audio_src_srv_resource_manager_event_t event)
{
    bt_sink_srv_dual_ant_context_t *ctx = bt_sink_srv_dual_ant_get_context();
    bt_sink_srv_report_id("[sink][dual_ant] mic_resource_callback handle:0x%x event:0x%x", 2, handle, event);

    switch (event) {
        case AUDIO_SRC_SRV_EVENT_TAKE_SUCCESS: {
            if (handle == ctx->mic_transmit.resource_handle) {
                bt_sink_srv_dual_ant_start_transmitter(BT_SINK_DUAL_ANT_TRANSMITTER_MIC);
            }
            break;
        }

        case AUDIO_SRC_SRV_EVENT_TAKE_REJECT: {
            break;
        }

        case AUDIO_SRC_SRV_EVENT_GIVE_SUCCESS: {
            break;
        }

        case AUDIO_SRC_SRV_EVENT_SUSPEND: {
            break;
        }

        default: {
            break;
        }
    }
}
