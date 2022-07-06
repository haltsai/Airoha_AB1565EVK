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

/**
 * File: app_usb_audio_idle_utils.c
 *
 * Description:
 * This file define the common function of usb audio.
 */

#include "app_usb_audio_utils.h"
#include "app_usb_audio_idle_activity.h"
#include "app_usb_audio_activity.h"
#include "hal_nvic.h"

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
#include "audio_source_control.h"
#include "audio_src_srv_resource_manager_config.h"
#include "audio_src_srv.h"
#endif

#define USB_AUDIO_DEFAULT_VOLUME 80
#define BT_ULL_UAC_DATA_LEN 32
#define USB_AUDIO_MAX_VOLUME 100
#define USB_AUDIO_MAX_LEVEL 15
#define USB_AUDIO_RATIO_MAX 100
#define USB_AUDIO_DEFAULT_SAMPLE_RATE 48000

/*========================================================================================*/
/*                                classcial usb audio                                     */
/*========================================================================================*/
typedef struct {
    bool started;
    bool is_waitting_signal;
    usb_audio_action_t pending_action;
    bool need_resume;
} usb_audio_context;

static usb_audio_context g_usb_ctx = {
    false,
    false,
    USB_AUDIO_ACTION_UNKNOWN,
    false
};

static void usb_audio_start_stop(usb_audio_action_t action);

static void audio_usb_audio_callback(audio_usb_audio_event_t event, void *user_data)
{
    USB_AUDIO_LOG_I("usb audio event: %d", 1, event);
    ui_shell_send_event(true, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_SWITCH_USB_AUDIO,
                        APPS_EVENTS_INTERACTION_USB_AUDIO_EV,
                        (void*)event, 0, NULL, 0);
}

static void _check_waiting_sig_and_pending_act()
{
    if (g_usb_ctx.is_waitting_signal) {
        g_usb_ctx.is_waitting_signal = false;
        if (g_usb_ctx.pending_action != USB_AUDIO_ACTION_UNKNOWN) {
            usb_audio_start_stop(g_usb_ctx.pending_action);
            g_usb_ctx.pending_action = USB_AUDIO_ACTION_UNKNOWN;
        }
    }
}

static void audio_usb_audio_event(audio_usb_audio_event_t event)
{
    static audio_usb_audio_event_t s_last_event = AUDIO_USB_AUDIO_EVENT_ERROR;
    USB_AUDIO_LOG_I("usb audio event, e:%d, w: %d, p: %d, c:%d", 4,
        event, g_usb_ctx.is_waitting_signal, g_usb_ctx.pending_action, g_usb_ctx.started);
    switch (event) {
        case AUDIO_USB_AUDIO_EVENT_ERROR:
            USB_AUDIO_LOG_E("usb audio event error!", 0);
            break;
        case AUDIO_USB_AUDIO_EVENT_PLAYING:
            _check_waiting_sig_and_pending_act();
            break;
        case AUDIO_USB_AUDIO_EVENT_STOPED:
            if (s_last_event == AUDIO_USB_AUDIO_EVENT_SUSPENDED) {
                USB_AUDIO_LOG_I("handle the suspend case!", 0);
                _check_waiting_sig_and_pending_act();
                break;
                #if 0
                g_usb_ctx.is_waitting_signal = false;
                if (g_usb_ctx.pending_action == USB_AUDIO_ACTION_UNKNOWN) {
                    /* There is no event callback on this call, it's used to clean the context */
                    usb_audio_start_stop(USB_AUDIO_ACTION_CLOSE);
                    g_usb_ctx.is_waitting_signal = false;
                    g_usb_ctx.need_resume = true;
                } else {
                    /* There is no event callback on this call, it's used to clean the context */
                    usb_audio_start_stop(USB_AUDIO_ACTION_CLOSE);
                    g_usb_ctx.is_waitting_signal = false;
                    /* Do the pending action. */
                    usb_audio_start_stop(g_usb_ctx.pending_action);
                    g_usb_ctx.pending_action = USB_AUDIO_ACTION_UNKNOWN;
                }
                #endif
            } else {
                _check_waiting_sig_and_pending_act();
            }
            break;
        case AUDIO_USB_AUDIO_EVENT_SUSPENDED:
            break;
        case AUDIO_USB_AUDIO_EVENT_REJECTED:
            g_usb_ctx.is_waitting_signal = false;
            if (g_usb_ctx.pending_action == USB_AUDIO_ACTION_UNKNOWN) {
                /* There is no event callback on this call, it's used to clean the context */
                usb_audio_start_stop(USB_AUDIO_ACTION_CLOSE);
                g_usb_ctx.is_waitting_signal = false;
                g_usb_ctx.need_resume = true;
            } else {
                /* There is no event callback on this call, it's used to clean the context */
                usb_audio_start_stop(USB_AUDIO_ACTION_CLOSE);
                g_usb_ctx.is_waitting_signal = false;
                /* Do the pending action. */
                usb_audio_start_stop(g_usb_ctx.pending_action);
                g_usb_ctx.pending_action = USB_AUDIO_ACTION_UNKNOWN;
            }
            break;
        case AUDIO_USB_AUDIO_EVENT_EXCEPTION:
            break;
    }

    s_last_event = event;
}

static audio_usb_audio_config_t g_audio_config = {
    audio_usb_audio_callback,
    NULL
};

static void usb_audio_start_stop(usb_audio_action_t action) {
    audio_usb_audio_status_t ret = AUDIO_USB_AUDIO_STATUS_ERROR;

    USB_AUDIO_LOG_I("usb audio start stop, a: %d, w: %d, p: %d, c:%d", 4,
        action, g_usb_ctx.is_waitting_signal, g_usb_ctx.pending_action, g_usb_ctx.started);

    if (g_usb_ctx.is_waitting_signal) {
        g_usb_ctx.pending_action = action;
        return;
    }

    if (g_usb_ctx.need_resume) {
        g_usb_ctx.need_resume = false;
    }

    if (action == USB_AUDIO_ACTION_OPEN) {
        if (g_usb_ctx.started) {
            return;
        }
        /* init and start usb audio */
        do {
            ret = audio_usb_audio_control_init(&g_audio_config);
            if (ret != AUDIO_USB_AUDIO_STATUS_SUCCESS) {
                USB_AUDIO_LOG_I("usb audio init fail: %d", 1, ret);
                break;
            }
            ret = audio_usb_audio_control_start();
        } while(0);
    } else if (action == USB_AUDIO_ACTION_CLOSE) {
        if (!g_usb_ctx.started) {
            return;
        }
        /* stop and deinit usb audio */
        do {
            ret = audio_usb_audio_control_stop();
            if (ret != AUDIO_USB_AUDIO_STATUS_SUCCESS) {
                USB_AUDIO_LOG_I("usb audio stop fail: %d", 1, ret);
                break;
            }
            ret = audio_usb_audio_control_deinit();
        } while(0);
    }

    USB_AUDIO_LOG_I("usb audio start stop, result: %d", 1, ret);
    if (ret == AUDIO_USB_AUDIO_STATUS_SUCCESS) {
        g_usb_ctx.is_waitting_signal = true;
        g_usb_ctx.started = action == USB_AUDIO_ACTION_OPEN ? true : false;
        g_usb_ctx.pending_action = USB_AUDIO_ACTION_UNKNOWN;
    }
}

void app_usb_audio_resume_after_line_in_common() {
    if (g_usb_ctx.need_resume) {
        USB_AUDIO_LOG_I("try to resume common usb audio after line in", 0);
        app_usb_audio_action(USB_AUDIO_IN_PORT1, USB_AUDIO_ACTION_OPEN, 0);
    }
}

bool app_usb_audio_bt_state_event(ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool do_resume = false;

    if (event_id == BT_SINK_SRV_EVENT_STATE_CHANGE) {
        bt_sink_srv_state_change_t *param = (bt_sink_srv_state_change_t *) extra_data;
        /* Try to resume USB audio after the call is end. */
        if ((param->previous >= BT_SINK_SRV_STATE_INCOMING) && (param->current <= BT_SINK_SRV_STATE_CONNECTED)) {
            do_resume = g_usb_ctx.need_resume;
        }
        if (do_resume) {
            /* Delay 2500 ms to resume usb audio to waitting HFP end completed. */
            app_usb_audio_action(USB_AUDIO_IN_PORT1, USB_AUDIO_ACTION_OPEN, 2500);
        }
    }
    return false;
}

/*========================================================================================*/
/*                                  common function                                       */
/*========================================================================================*/
#define USB_AUDIO_OUT_FRAME_SIZE 960
static bool s_line_in_running = false;

#if defined(AIR_USB_AUDIO_MIX_ENABLE) || defined(AIR_USB_AUDIO_OUT_ENABLE)
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
static void app_usb_out_request_source();
static void app_usb_in_request_source();
static void app_usb_out_release_source();
static void app_usb_in_release_source();
static void app_usb_in_audio_source_event(uint32_t event);
#endif
#endif

#if defined(AIR_USB_AUDIO_MIX_ENABLE) || defined(AIR_USB_AUDIO_OUT_ENABLE)
typedef enum {
    TRANSMITTER_USR_USB_AUDIO_MIX,
    TRANSMITTER_USR_USB_AUDIO_MIX1,
    TRANSMITTER_USR_USB_OUT,
    TRANSMITTER_USR_MAX
} TRANSMITTER_USR_T;

typedef struct {
    TRANSMITTER_USR_T type;
    audio_transmitter_id_t transmitter_id;
    bool is_waitting_signal;
    usb_audio_action_t pending_action;
    usb_audio_action_t last_action;
    bool started;
    audio_transmitter_event_t suspend_by;
    volatile uint8_t volume_l;
    volatile uint8_t volume_r;
    volatile uint8_t mix_ratio;
    volatile bool mute;
    uint32_t sample_rate;
    uint32_t des_sample_rate;
    uint32_t frame_size;
} transmitter_usr;

static transmitter_usr s_usr_group[TRANSMITTER_USR_MAX];
static volatile uint32_t s_data_frame_count = 0;

static void app_usb_audio_transmitter_event_callback(audio_transmitter_event_t event, void *data, void *user_data)
{
    transmitter_usr* ctx = (transmitter_usr*)user_data;
    uint32_t shell_event = ((ctx->type & 0xff) << 8) | (event & 0xff);
#if 1
    /* Avoid usb out's transmitter buffer full. */
    if (ctx == &s_usr_group[TRANSMITTER_USR_USB_OUT]) {
        if (event == AUDIO_TRANSMITTER_EVENT_DATA_NOTIFICATION) {
            static uint32_t log_count = 0;
            if (log_count++ % 20 == 0) {
                USB_AUDIO_LOG_I("audio transmitter event, %d:%d, size=%d", 3, ctx->type, event, ctx->frame_size);
            }
            uint32_t mask;
            uint32_t count_temp = 0;
            hal_nvic_save_and_set_interrupt_mask(&mask);
            s_data_frame_count ++;
            count_temp = s_data_frame_count;
            hal_nvic_restore_interrupt_mask(mask);
            /* If the counter > 2, the data is not readout by usb irq. */
            if (count_temp > 2) {
                uint8_t *data1 = NULL;
                uint32_t len = 0;
                audio_transmitter_get_read_information(ctx->transmitter_id, &data1, &len);
                if (len > 0) {
                    audio_transmitter_read_done(ctx->transmitter_id, len);
                    hal_nvic_save_and_set_interrupt_mask(&mask);
                    s_data_frame_count --;
                    hal_nvic_restore_interrupt_mask(mask);
                    USB_AUDIO_LOG_I("audio data droup, %d", 1, count_temp);
                }
            }
            return;
        } else if (event == AUDIO_TRANSMITTER_EVENT_START_SUCCESS) {
            s_data_frame_count = 0;
        }
    }
#endif
    ui_shell_send_event(true, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_SWITCH_USB_AUDIO,
                        APPS_EVENTS_INTERACTION_USB_AUDIO_TRANSMITTER,
                        (void*)shell_event, 0, NULL, 0);
}

void usb_audio_tx_ready()
{
    static uint32_t s_voice_data_offset = 0;
    static uint32_t s_voice_data_len = 0;
    static uint8_t *s_voice_data = NULL;
    uint32_t mask;

    transmitter_usr *ctx = &s_usr_group[TRANSMITTER_USR_USB_OUT];
    //USB_AUDIO_LOG_I("tx ready", 0);
    if (ctx->started) {
        if (s_voice_data == NULL) {
            //USB_AUDIO_LOG_I("read data info", 0);
            audio_transmitter_get_read_information(ctx->transmitter_id, &s_voice_data, &s_voice_data_len);
            hal_nvic_save_and_set_interrupt_mask(&mask);
            if (s_data_frame_count > 0) {
                s_data_frame_count--;
            }
            hal_nvic_restore_interrupt_mask(mask);
            s_voice_data_offset = 0;
        }
        if (s_voice_data == NULL) {
            hal_usb_set_endpoint_tx_ready(1);
            return;
        }

        if (!ctx->mute) {
            hal_usb_write_endpoint_fifo(0x01, ctx->frame_size, (void*)(s_voice_data + s_voice_data_offset));
        }
        s_voice_data_offset += ctx->frame_size;

        if (s_voice_data_offset >= s_voice_data_len) {
            //USB_AUDIO_LOG_I("data read done-> %d,%d", 2, ctx->frame_size, s_voice_data_len);
            audio_transmitter_read_done(ctx->transmitter_id, s_voice_data_len);
            s_voice_data = NULL;
            s_voice_data_offset = 0;
        }
    }
    hal_usb_set_endpoint_tx_ready(1);
}

static void app_usb_audio_transmitter_init(TRANSMITTER_USR_T type, audio_transmitter_config_t *cfg, uint32_t sample_rate)
{
    static bool s_is_init_mix_ratio_set[TRANSMITTER_USR_MAX] = {false};

    cfg->user_data = &s_usr_group[type];
    audio_transmitter_id_t id = audio_transmitter_init(cfg);
    USB_AUDIO_LOG_I("audio transmitter init, type:%d, re: %d", 2, type, id);
    s_usr_group[type].type = type;
    s_usr_group[type].transmitter_id = id;
    s_usr_group[type].is_waitting_signal = false;
    s_usr_group[type].pending_action = USB_AUDIO_ACTION_UNKNOWN;
    s_usr_group[type].last_action = USB_AUDIO_ACTION_UNKNOWN;
    s_usr_group[type].started = false;
    s_usr_group[type].suspend_by = AUDIO_TRANSMITTER_EVENT_START_SUCCESS;
    s_usr_group[type].mute = false;
    s_usr_group[type].volume_l = USB_AUDIO_DEFAULT_VOLUME;
    s_usr_group[type].volume_r = USB_AUDIO_DEFAULT_VOLUME;
    s_usr_group[type].sample_rate = sample_rate;
    s_usr_group[type].des_sample_rate = sample_rate;
    s_usr_group[type].frame_size = sample_rate == 48000 ? 96 : 32;
    if (!s_is_init_mix_ratio_set[type]) {
        s_is_init_mix_ratio_set[type] = true;
        s_usr_group[type].mix_ratio = USB_AUDIO_RATIO_MAX;
    }
}

static void app_usb_audio_transmitter_op(TRANSMITTER_USR_T type, usb_audio_action_t action) {
    audio_transmitter_status_t sta = AUDIO_TRANSMITTER_STATUS_FAIL;
    transmitter_usr* ctx = &s_usr_group[type];
    USB_AUDIO_LOG_I("audio transmitter OP, type:%d, pd: %d, w: %d, p: %d, c:%d", 5,
        type, action, ctx->is_waitting_signal, ctx->pending_action, ctx->started);
    if (ctx->is_waitting_signal) {
        ctx->pending_action = action;
        return;
    }

    if (action == USB_AUDIO_ACTION_OPEN) {
        if (ctx->started && ctx->suspend_by < AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_HFP) {
            return;
        }
        if (s_line_in_running) {
            USB_AUDIO_LOG_I("ignore open action due to line in in running", 0);
            ctx->started = true;
            ctx->suspend_by = AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_LINE_IN;
            return;
        }
        ctx->suspend_by = AUDIO_TRANSMITTER_EVENT_START_SUCCESS;
        /* check and update the sample rate. */
        USB_AUDIO_LOG_I("check and update sample rate from %d to %d", 2, ctx->sample_rate, ctx->des_sample_rate);
        if (ctx->sample_rate != ctx->des_sample_rate) {
            static audio_transmitter_config_t config;
            memset((void*) &config, 0, sizeof(audio_transmitter_config_t));
            if (type == TRANSMITTER_USR_USB_OUT) {
                audio_transmitter_deinit(ctx->transmitter_id);
                config.scenario_type = AUDIO_TRANSMITTER_WIRED_AUDIO;
                config.scenario_sub_id = AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT;
                config.msg_handler = app_usb_audio_transmitter_event_callback;
                config.scenario_config.wired_audio_config.usb_out_config.usb_codec_type = AUDIO_DSP_CODEC_TYPE_PCM;
                config.scenario_config.wired_audio_config.usb_out_config.usb_codec_param.pcm.sample_rate = ctx->des_sample_rate;
                config.user_data = &s_usr_group[TRANSMITTER_USR_USB_OUT];
                audio_transmitter_id_t id = audio_transmitter_init(&config);
                USB_AUDIO_LOG_I("audio transmitter reinit, type:%d, re: %d", 2, type, id);
                ctx->transmitter_id = id;
                ctx->sample_rate = ctx->des_sample_rate;
                ctx->frame_size = ctx->sample_rate == 48000 ? 96 : 32;
            }
        }
        sta = audio_transmitter_start(ctx->transmitter_id);
        if (type == TRANSMITTER_USR_USB_OUT) {
            hal_usb_set_endpoint_tx_ready(1);
        }
    } else if (action == USB_AUDIO_ACTION_CLOSE) {
        if (!ctx->started) {
            return;
        }
        if (ctx->suspend_by >= AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_HFP) {
            USB_AUDIO_LOG_I("usb audio already suspend by: %d", 1, ctx->suspend_by);
            ctx->started = false;
            ctx->pending_action = USB_AUDIO_ACTION_UNKNOWN;
            ctx->suspend_by = AUDIO_TRANSMITTER_EVENT_START_SUCCESS;
            return;
        }
        /* The transmitter already stopped, clear resume flag. */
        ctx->suspend_by = AUDIO_TRANSMITTER_EVENT_START_SUCCESS;
        sta = audio_transmitter_stop(ctx->transmitter_id);
    }

    USB_AUDIO_LOG_I("audio transmitter OP, result: %d", 1, sta);
    if (sta == AUDIO_TRANSMITTER_STATUS_SUCCESS) {
        ctx->is_waitting_signal = true;
        ctx->started = action == USB_AUDIO_ACTION_OPEN ? true : false;
        ctx->pending_action = USB_AUDIO_ACTION_UNKNOWN;
    } else if (action != USB_AUDIO_ACTION_OPEN) {
        ctx->started = false;
        ctx->pending_action = USB_AUDIO_ACTION_UNKNOWN;
    }
}

static void __app_usb_audio_set_transmitter_volume(TRANSMITTER_USR_T type) {
    audio_transmitter_status_t sta = AUDIO_TRANSMITTER_STATUS_FAIL;
    audio_transmitter_runtime_config_t config;
    transmitter_usr* ctx = &s_usr_group[type];
    float volume_l = ((float)ctx->volume_l * USB_AUDIO_MAX_LEVEL) / USB_AUDIO_MAX_VOLUME + 0.5f;
    float volume_r = ((float)ctx->volume_r * USB_AUDIO_MAX_LEVEL) / USB_AUDIO_MAX_VOLUME + 0.5f;

    config.wired_audio_runtime_config.vol_level.vol_level_l = ctx->mute ? 0 : (uint8_t)volume_l;
    config.wired_audio_runtime_config.vol_level.vol_level_r = ctx->mute ? 0 : (uint8_t)volume_r;
    config.wired_audio_runtime_config.vol_level.vol_ratio = ctx->mix_ratio;
    sta = audio_transmitter_set_runtime_config(ctx->transmitter_id,
        type == TRANSMITTER_USR_USB_OUT ? WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_VOICE_DUL : WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_MUSIC_DUL, &config);
    USB_AUDIO_LOG_I("volume set t:%d, m:0x%x, v:%d-%d, v1: %d-%d, ratio:%d, result:%d", 8,
        type, ctx->mute, ctx->volume_l, ctx->volume_r,(uint8_t)volume_l, (uint8_t)volume_r, ctx->mix_ratio, sta);
}

static void app_usb_audio_transmitter_event(ui_shell_activity_t *self, uint16_t event)
{
    TRANSMITTER_USR_T type = (event & 0xff00) >> 8;
    audio_transmitter_event_t ev = event & 0xff;
    transmitter_usr* ctx = &s_usr_group[type];

    if (ev != AUDIO_TRANSMITTER_EVENT_DATA_NOTIFICATION) {
        USB_AUDIO_LOG_I("audio transmitter ev, type:%d, ev: %d, w: %d, p: %d, c: %d", 5, type, ev, ctx->is_waitting_signal,
            ctx->pending_action, ctx->started);
    }

    switch(ev) {
        case AUDIO_TRANSMITTER_EVENT_START_SUCCESS:
            __app_usb_audio_set_transmitter_volume(type);
#ifdef AIR_USB_HID_MEDIA_CTRL_ENABLE
            USB_AUDIO_LOG_I("start usb audio activity due to usb audio started", 0);
            if (type == TRANSMITTER_USR_USB_AUDIO_MIX || type == TRANSMITTER_USR_USB_AUDIO_MIX1) {
                ui_shell_start_activity(self, app_usb_audio_activity, ACTIVITY_PRIORITY_LOW, NULL, 0);
            }
#endif
        case AUDIO_TRANSMITTER_EVENT_START_FAIL:
        case AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS:
        case AUDIO_TRANSMITTER_EVENT_SET_RUNTIME_CONFIG_SUCCESS:
            if (ctx->is_waitting_signal) {
                ctx->is_waitting_signal = false;
                if (ctx->pending_action != USB_AUDIO_ACTION_UNKNOWN) {
                    app_usb_audio_transmitter_op(type, ctx->pending_action);
                    ctx->pending_action = USB_AUDIO_ACTION_UNKNOWN;
                }
            }
            /* Enable or disable for usb out */
            if (type == TRANSMITTER_USR_USB_OUT) {
                bt_sink_srv_am_result_t ret = AUD_EXECUTION_SUCCESS;
                if (ev == AUDIO_TRANSMITTER_EVENT_START_SUCCESS) {
                    am_audio_side_tone_enable();
                } else if (ev == AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS) {
                    am_audio_side_tone_disable();
                }
                USB_AUDIO_LOG_I("enable or disable side tone for usb out", 1, ret);
            }
            break;
        case AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_HFP:
        case AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_HFP:
        case AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_RECORDER:
        case AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_RECORDER:
        case AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_A2DP:
        case AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_A2DP:
        case AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_LINE_IN:
        case AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_LINE_OUT:
        case AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_LE_CALL:
        case AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_LE_CALL:
            if (ctx->is_waitting_signal) {
                ctx->is_waitting_signal = false;
            }
            /* If the close operation happend before reject or suspend, do not resume it. */
            if (ctx->started && ctx->pending_action != USB_AUDIO_ACTION_CLOSE) {
                ctx->suspend_by = ev;
            }
            /* If close operation in pending, ignore it. */
            if (ctx->pending_action == USB_AUDIO_ACTION_CLOSE) {
                ctx->pending_action = USB_AUDIO_ACTION_UNKNOWN;
            }
            /* Check and disable side tone for usb out. */
            if (type == TRANSMITTER_USR_USB_OUT) {
                bt_sink_srv_am_result_t ret = am_audio_side_tone_disable();
                USB_AUDIO_LOG_I("enable side tone for usb out", 1, ret);
            }
            break;
    }

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
    if (type == TRANSMITTER_USR_USB_OUT && ev == AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS) {
        if (s_usr_group[TRANSMITTER_USR_USB_OUT].last_action == USB_AUDIO_ACTION_CLOSE) {
            app_usb_out_release_source();
        } else {
            USB_AUDIO_LOG_I("do not release resource due to the suspend case", 0);
        }
    } else if (ev == AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS) {
        if (!s_usr_group[TRANSMITTER_USR_USB_AUDIO_MIX].started && !s_usr_group[TRANSMITTER_USR_USB_AUDIO_MIX1].started)
            app_usb_in_release_source();
    }
#endif
}

void app_usb_resume_after_line_in() {
    if (app_usb_audio_scenario_type() == USB_AUDIO_SCENARIO_TYPE_MIX_OUT) {
        usb_audio_port_t port = 0;
        if (s_usr_group[TRANSMITTER_USR_USB_AUDIO_MIX].suspend_by == AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_LINE_IN) {
            port |= USB_AUDIO_IN_PORT1;
        }
#ifdef AIR_USB_IN_PORT2_ENABLE
        if (s_usr_group[TRANSMITTER_USR_USB_AUDIO_MIX1].suspend_by == AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_LINE_IN) {
            port |= USB_AUDIO_IN_PORT2;
        }
#endif
#ifdef AIR_USB_AUDIO_OUT_ENABLE
        if (s_usr_group[TRANSMITTER_USR_USB_OUT].suspend_by == AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_LINE_OUT) {
            port |= USB_AUDIO_OUT_PORT1;
        }
#endif
        USB_AUDIO_LOG_I("resume usb stream after line in : %d", 1, port);
        app_usb_audio_action(port, USB_AUDIO_ACTION_OPEN, 0);
    }
}

#ifdef AIR_LE_AUDIO_ENABLE
void app_usb_resume_after_le_audio() {
    if (app_usb_audio_scenario_type() == USB_AUDIO_SCENARIO_TYPE_MIX_OUT) {
        usb_audio_port_t port = 0;
        if (s_usr_group[TRANSMITTER_USR_USB_AUDIO_MIX].suspend_by == AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_LE_CALL ||
            s_usr_group[TRANSMITTER_USR_USB_AUDIO_MIX].suspend_by == AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_LE_CALL) {
            port |= USB_AUDIO_IN_PORT1;
        }
#ifdef AIR_USB_IN_PORT2_ENABLE
        if (s_usr_group[TRANSMITTER_USR_USB_AUDIO_MIX1].suspend_by == AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_LE_CALL ||
            s_usr_group[TRANSMITTER_USR_USB_AUDIO_MIX1].suspend_by == AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_LE_CALL) {
            port |= USB_AUDIO_IN_PORT2;
        }
#endif
#ifdef AIR_USB_AUDIO_OUT_ENABLE
        if (s_usr_group[TRANSMITTER_USR_USB_OUT].suspend_by == AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_LE_CALL ||
            s_usr_group[TRANSMITTER_USR_USB_OUT].suspend_by == AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_LE_CALL) {
            port |= USB_AUDIO_OUT_PORT1;
        }
#endif
        USB_AUDIO_LOG_I("resume usb stream after leaudio : %d", 1, port);
        app_usb_audio_action(port, USB_AUDIO_ACTION_OPEN, 0);
    }
}

#endif

void app_usb_audio_mix_rotatry_change(uint8_t mix_arg) {
    s_usr_group[TRANSMITTER_USR_USB_AUDIO_MIX].mix_ratio = (mix_arg <= ULL_MIX_RATIO_BALANCED_LEVEL) ?
                    100 :
                    100 * (ULL_MIX_RATIO_CHAT_MAX_LEVEL - mix_arg) / (ULL_MIX_RATIO_CHAT_MAX_LEVEL - ULL_MIX_RATIO_BALANCED_LEVEL);
    s_usr_group[TRANSMITTER_USR_USB_AUDIO_MIX1].mix_ratio = (mix_arg >= ULL_MIX_RATIO_BALANCED_LEVEL) ?
                    100 :
                    100 * (mix_arg - ULL_MIX_RATIO_GAME_MAX_LEVEL) / (ULL_MIX_RATIO_BALANCED_LEVEL - ULL_MIX_RATIO_GAME_MAX_LEVEL);
    __app_usb_audio_set_transmitter_volume(TRANSMITTER_USR_USB_AUDIO_MIX);
    __app_usb_audio_set_transmitter_volume(TRANSMITTER_USR_USB_AUDIO_MIX1);
}

void app_usb_audio_set_mix_ratio(uint8_t game_ratio, uint8_t chart_ratio) {
    s_usr_group[TRANSMITTER_USR_USB_AUDIO_MIX].mix_ratio = game_ratio;
    s_usr_group[TRANSMITTER_USR_USB_AUDIO_MIX1].mix_ratio = chart_ratio;
    __app_usb_audio_set_transmitter_volume(TRANSMITTER_USR_USB_AUDIO_MIX);
    __app_usb_audio_set_transmitter_volume(TRANSMITTER_USR_USB_AUDIO_MIX1);
}

void app_usb_audio_set_volume(usb_audio_port_t port, uint8_t channel, uint8_t volume, bool mute)
{
    TRANSMITTER_USR_T usr = TRANSMITTER_USR_MAX;
    switch (port) {
        case USB_AUDIO_IN_PORT1:
            usr = TRANSMITTER_USR_USB_AUDIO_MIX;
            break;
        case USB_AUDIO_IN_PORT2:
            usr = TRANSMITTER_USR_USB_AUDIO_MIX1;
            break;
        case USB_AUDIO_OUT_PORT1:
            usr = TRANSMITTER_USR_USB_OUT;
            break;
        default:
            return;
    }

    if (mute) {
        s_usr_group[usr].mute = mute;
    } else {
        s_usr_group[usr].mute = false;
        if (channel == USB_AUDIO_VOLUME_CHANNEL_L) {
            s_usr_group[usr].volume_l = volume;
        } else if (channel == USB_AUDIO_VOLUME_CHANNEL_R) {
            s_usr_group[usr].volume_r = volume;
        }
    }

    ui_shell_send_event(true, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_SWITCH_USB_AUDIO,
                        APPS_EVENTS_INTERACTION_USB_VOLUME_SET,
                        (void*)usr, 0, NULL, 0);
}

void app_usb_audio_set_sampling_rate(usb_audio_port_t port, uint32_t rate)
{
    TRANSMITTER_USR_T usr = TRANSMITTER_USR_MAX;
    switch (port) {
        case USB_AUDIO_IN_PORT1:
            usr = TRANSMITTER_USR_USB_AUDIO_MIX;
            break;
        case USB_AUDIO_IN_PORT2:
            usr = TRANSMITTER_USR_USB_AUDIO_MIX1;
            break;
        case USB_AUDIO_OUT_PORT1:
            usr = TRANSMITTER_USR_USB_OUT;
            break;
        default:
            return;
    }

    /* delay the sample rate to the front of start. */
    s_usr_group[usr].des_sample_rate = rate;
    USB_AUDIO_LOG_I("set semphr for %d, old=%d, des=%d", 3, usr, s_usr_group[usr].sample_rate, s_usr_group[usr].des_sample_rate);
}

#endif

#if defined(AIR_USB_AUDIO_OUT_ENABLE)
bool app_usb_audio_handle_sink_event(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len) {

    uint32_t i = 0;
    if (event_id == BT_SINK_SRV_EVENT_STATE_CHANGE) {
        bt_sink_srv_state_change_t *param = (bt_sink_srv_state_change_t *) extra_data;
        if ((param->previous >= BT_SINK_SRV_STATE_STREAMING) && (param->current <= BT_SINK_SRV_STATE_CONNECTED)) {
            USB_AUDIO_LOG_I("try to resume after hfp or a2dp", 0);
            for (i = 0; i < TRANSMITTER_USR_MAX; i++) {
                switch (s_usr_group[i].suspend_by) {
                    case AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_HFP:
                    case AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_HFP:
                    case AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_A2DP:
                    case AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_A2DP:
                        USB_AUDIO_LOG_I("resume: %d after suspend by:%d", 2, s_usr_group[i].suspend_by);
                        app_usb_audio_transmitter_op((TRANSMITTER_USR_T)i, USB_AUDIO_ACTION_OPEN);
                        break;
                }
            }
        }
    }
    return false;
}
#endif

#if defined(AIR_USB_AUDIO_MIX_ENABLE) || defined(AIR_USB_AUDIO_OUT_ENABLE)
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)

void* usb_out_audio_source_handle = NULL;
void* usb_in_audio_source_handle = NULL;

static void usb_out_audio_source_callback(audio_source_control_event_type type, uint32_t sub_event, void* usr_data)
{
    if (usr_data == NULL) {
        USB_AUDIO_LOG_I("usb_out_audio_source_callback invalid usr data.", 0);
    }

    USB_AUDIO_LOG_I("usb_out_audio_source_callback type=%d, sub_event=%d.", 2, type, sub_event);
    uint32_t res_event = ((type << 8) & 0xFF00) | (sub_event & 0xFF);
    uint32_t shell_event = TRANSMITTER_USR_USB_OUT << 16 | (res_event & 0xFFFF);
    ui_shell_send_event(true, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_SWITCH_USB_AUDIO,
                        APPS_EVENTS_INTERACTION_USB_AUDIO_SRC_CTRL_EV,
                        (void*)shell_event, 0, NULL, 0);
}

static void usb_in_audio_source_callback(audio_source_control_event_type type, uint32_t sub_event, void* usr_data)
{
    if (usr_data == NULL) {
        USB_AUDIO_LOG_I("usb_out_audio_source_callback invalid usr data.", 0);
    }
    USB_AUDIO_LOG_I("usb_out_audio_source_callback type=%d, sub_event=%d.", 2, type, sub_event);

    usb_audio_action_t last_action = s_usr_group[TRANSMITTER_USR_USB_AUDIO_MIX].last_action;
    USB_AUDIO_LOG_I("usb in1 request notify last_action=%d.", 1, last_action);
    uint32_t res_event = ((type << 8) & 0xFF00) | (sub_event & 0xFF);
    uint32_t shell_event = TRANSMITTER_USR_USB_AUDIO_MIX << 16 | (res_event & 0xFFFF);
    ui_shell_send_event(true, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_SWITCH_USB_AUDIO,
                        APPS_EVENTS_INTERACTION_USB_AUDIO_SRC_CTRL_EV,
                        (void*)shell_event, 0, NULL, 0);

    last_action = s_usr_group[TRANSMITTER_USR_USB_AUDIO_MIX1].last_action;
    USB_AUDIO_LOG_I("usb in2 request notify last_action=%d.", 1, last_action);
    shell_event = TRANSMITTER_USR_USB_AUDIO_MIX1 << 16 | (res_event & 0xFFFF);
    ui_shell_send_event(true, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_SWITCH_USB_AUDIO,
                    APPS_EVENTS_INTERACTION_USB_AUDIO_SRC_CTRL_EV,
                    (void*)shell_event, 0, NULL, 0);
}

void app_usb_audio_source_ctrl_init()
{
    audio_source_control_cfg_t cfg = {
        AUDIO_SRC_SRV_RESOURCE_TYPE_MIC,
        AUDIO_SRC_SRV_RESOURCE_TYPE_MIC_USER_RECORD_PRIORITY,
        AUDIO_SOURCE_CONTROL_USR_USB_OUT,
        (uint8_t*)"usb_out",
        NULL,
        usb_out_audio_source_callback
    };
    usb_out_audio_source_handle = audio_source_control_register(&cfg);
    if (usb_out_audio_source_handle == NULL) {
        USB_AUDIO_LOG_E("usb out register audio source control failed.", 0);
        return;
    } else {
        USB_AUDIO_LOG_I("usb out register audio source control success.", 0);
    }

    cfg.res_type = AUDIO_SRC_SRV_RESOURCE_TYPE_WIRED_AUDIO;
    cfg.usr = AUDIO_SOURCE_CONTROL_USR_USB_IN;
    cfg.usr_name = (uint8_t*)"usb_in";
    cfg.request_notify = usb_in_audio_source_callback;
    usb_in_audio_source_handle = audio_source_control_register(&cfg);
    if (usb_in_audio_source_handle == NULL) {
        USB_AUDIO_LOG_E("usb in register audio source control failed.", 0);
        return;
    } else {
        USB_AUDIO_LOG_I("usb in register audio source control success.", 0);
    }
}

static void app_usb_out_request_source()
{
    audio_source_control_result_t result = audio_source_control_cmd(usb_out_audio_source_handle, AUDIO_SOURCE_CONTROL_CMD_REQUEST_RES, AUDIO_SOURCE_CONTROL_CMD_DEST_LOCAL);
    USB_AUDIO_LOG_I("usb out request audio result: %d.", 1, result);
}

static void app_usb_in_request_source()
{
    audio_source_control_result_t result = audio_source_control_cmd(usb_in_audio_source_handle, AUDIO_SOURCE_CONTROL_CMD_REQUEST_RES, AUDIO_SOURCE_CONTROL_CMD_DEST_LOCAL);
    USB_AUDIO_LOG_I("usb in request audio result: %d.", 1, result);
}

static void app_usb_out_release_source()
{
    audio_source_control_result_t result = audio_source_control_cmd(usb_out_audio_source_handle, AUDIO_SOURCE_CONTROL_CMD_DEL_WAITTING_LIST, AUDIO_SOURCE_CONTROL_CMD_DEST_LOCAL);
    USB_AUDIO_LOG_I("usb out release audio, remove waiiting result: %d.", 1, result);
    result = audio_source_control_cmd(usb_out_audio_source_handle, AUDIO_SOURCE_CONTROL_CMD_RELEASE_RES, AUDIO_SOURCE_CONTROL_CMD_DEST_LOCAL);
    USB_AUDIO_LOG_I("usb out release audio result: %d.", 1, result);
}

static void app_usb_in_release_source()
{
    audio_source_control_result_t result = audio_source_control_cmd(usb_in_audio_source_handle, AUDIO_SOURCE_CONTROL_CMD_DEL_WAITTING_LIST, AUDIO_SOURCE_CONTROL_CMD_DEST_LOCAL);
    USB_AUDIO_LOG_I("usb in release audio result, remove waitting: %d.", 1, result);
    result = audio_source_control_cmd(usb_in_audio_source_handle, AUDIO_SOURCE_CONTROL_CMD_RELEASE_RES, AUDIO_SOURCE_CONTROL_CMD_DEST_LOCAL);
    USB_AUDIO_LOG_I("usb in release audio result: %d.", 1, result);
}

static void app_usb_in_audio_source_event(uint32_t event)
{
    uint32_t event_info = event & 0xFFFF;
    TRANSMITTER_USR_T usr_type = (event >> 16) & 0xFFFF;
    audio_source_control_event_type ev_type = (event_info >> 8) & 0xFF;

    USB_AUDIO_LOG_I("app_usb_in_audio_source_event type=%d, ev=%d", 2, ev_type, event_info & 0xFF);
    if (ev_type == AUDIO_SOURCE_CONTROL_EVENT_RES_CTRL) {
        audio_src_srv_resource_manager_event_t res_ev = (audio_src_srv_resource_manager_event_t)(event_info & 0xFF);
        switch (res_ev) {
            case AUDIO_SRC_SRV_EVENT_TAKE_ALREADY:
            case AUDIO_SRC_SRV_EVENT_TAKE_SUCCESS:
                if (s_usr_group[usr_type].last_action == USB_AUDIO_ACTION_OPEN) {
                    app_usb_audio_transmitter_op(usr_type, USB_AUDIO_ACTION_OPEN);
                }
                break;
            case AUDIO_SRC_SRV_EVENT_TAKE_REJECT:
                if (usr_type == TRANSMITTER_USR_USB_OUT) {
                    audio_source_control_cmd(usb_out_audio_source_handle, AUDIO_SOURCE_CONTROL_CMD_ADD_WAITTING_LIST, AUDIO_SOURCE_CONTROL_CMD_DEST_LOCAL);
                } else {
                    audio_source_control_cmd(usb_in_audio_source_handle, AUDIO_SOURCE_CONTROL_CMD_ADD_WAITTING_LIST, AUDIO_SOURCE_CONTROL_CMD_DEST_LOCAL);
                }
                break;
            case AUDIO_SRC_SRV_EVENT_SUSPEND:
                app_usb_audio_transmitter_op(usr_type, USB_AUDIO_ACTION_CLOSE);
                if (usr_type == TRANSMITTER_USR_USB_OUT) {
                    app_usb_out_release_source();
                    audio_source_control_cmd(usb_out_audio_source_handle, AUDIO_SOURCE_CONTROL_CMD_ADD_WAITTING_LIST, AUDIO_SOURCE_CONTROL_CMD_DEST_LOCAL);
                } else {
                    app_usb_in_release_source();
                    audio_source_control_cmd(usb_in_audio_source_handle, AUDIO_SOURCE_CONTROL_CMD_ADD_WAITTING_LIST, AUDIO_SOURCE_CONTROL_CMD_DEST_LOCAL);
                }
                break;
        }
    }
}
#endif
#endif

#ifdef AIR_USB_AUDIO_MIX_ENABLE
void app_usb_audio_mix_init() {
    audio_transmitter_config_t config;
    memset((void*) &config, 0, sizeof(audio_transmitter_config_t));
    config.scenario_type = AUDIO_TRANSMITTER_WIRED_AUDIO;
    config.scenario_sub_id = AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0;
    config.msg_handler = app_usb_audio_transmitter_event_callback;
    config.user_data = NULL;

    app_usb_audio_transmitter_init(TRANSMITTER_USR_USB_AUDIO_MIX, &config, 0);
#ifdef AIR_USB_IN_PORT2_ENABLE
    config.scenario_sub_id = AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1;
    app_usb_audio_transmitter_init(TRANSMITTER_USR_USB_AUDIO_MIX1, &config, 0);
#endif
}
#endif

#ifdef AIR_USB_AUDIO_OUT_ENABLE
void app_usb_out_init() {
    audio_transmitter_config_t config;
    memset((void*) &config, 0, sizeof(audio_transmitter_config_t));
    config.scenario_type = AUDIO_TRANSMITTER_WIRED_AUDIO;
    config.scenario_sub_id = AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT;
    config.msg_handler = app_usb_audio_transmitter_event_callback;
    config.scenario_config.wired_audio_config.usb_out_config.usb_codec_type = AUDIO_DSP_CODEC_TYPE_PCM;
    config.scenario_config.wired_audio_config.usb_out_config.usb_codec_param.pcm.sample_rate = USB_AUDIO_DEFAULT_SAMPLE_RATE;
    config.user_data = NULL;

    app_usb_audio_transmitter_init(TRANSMITTER_USR_USB_OUT, &config, USB_AUDIO_DEFAULT_SAMPLE_RATE);
}

#endif

void app_usb_audio_set_line_in_running_state(bool running)
{
    USB_AUDIO_LOG_I("set line in running state: %d", 1, running);
    s_line_in_running = running;
}

bool app_usb_audio_get_line_in_running_state()
{
    return s_line_in_running;
}
/*========================================================================================*/
/*                                     common logic                                       */
/*========================================================================================*/

bool app_usb_audio_is_open() {
    if (app_usb_audio_scenario_type() != USB_AUDIO_SCENARIO_TYPE_MIX_OUT) {
        return g_usb_ctx.started;
    } else {
#ifdef AIR_USB_AUDIO_MIX_ENABLE
        return s_usr_group[TRANSMITTER_USR_USB_AUDIO_MIX].started;
#endif
    }

    return false;
}

bool app_usb_out_is_open() {
#ifdef AIR_USB_AUDIO_OUT_ENABLE
    return s_usr_group[TRANSMITTER_USR_USB_OUT].started;
#endif
    return false;
}


void app_usb_audio_action(usb_audio_port_t port, usb_audio_action_t action, uint32_t delay_ms)
{
    uint32_t shell_event = 0;
    shell_event |= (port << 8 & 0xFF00);
    shell_event |= (action & 0xFF);

    //ui_shell_remove_event(EVENT_GROUP_SWITCH_USB_AUDIO, APPS_EVENTS_INTERACTION_USB_AUDIO_CMD);
    /* Use 500 ms to debounce. */
    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_SWITCH_USB_AUDIO,
                        APPS_EVENTS_INTERACTION_USB_AUDIO_CMD,
                        (void *)shell_event, 0, NULL, delay_ms);
}

bool app_usb_audio_event(ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    switch(event_id) {
        case APPS_EVENTS_INTERACTION_USB_AUDIO_EV: {
            audio_usb_audio_event_t event = (audio_usb_audio_event_t)extra_data;
            audio_usb_audio_event(event);
            break;
        }
        case APPS_EVENTS_INTERACTION_USB_AUDIO_CMD: {
            uint32_t shell_event = (uint32_t)extra_data;
            usb_audio_action_t action = shell_event & 0xFF;
            if (app_usb_audio_scenario_type() != USB_AUDIO_SCENARIO_TYPE_MIX_OUT) {
                usb_audio_start_stop(action);
            } else {
                usb_audio_port_t port = (shell_event >> 8) & 0xFF;
                port = port; /* avoid unused warning. */
#ifdef AIR_USB_AUDIO_MIX_ENABLE
                if (port & USB_AUDIO_IN_PORT1) {
                    s_usr_group[TRANSMITTER_USR_USB_AUDIO_MIX].last_action = action;
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
                    if (action == USB_AUDIO_ACTION_OPEN) {
                        app_usb_in_request_source();
                        
                    } else {
                        app_usb_audio_transmitter_op(TRANSMITTER_USR_USB_AUDIO_MIX, action);
                    }
#else
                    app_usb_audio_transmitter_op(TRANSMITTER_USR_USB_AUDIO_MIX, action);
#endif
                }
#ifdef AIR_USB_IN_PORT2_ENABLE
                if (port & USB_AUDIO_IN_PORT2) {
                    s_usr_group[TRANSMITTER_USR_USB_AUDIO_MIX1].last_action = action;
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
                    if (action == USB_AUDIO_ACTION_OPEN) {
                        app_usb_in_request_source();
                    } else {
                        app_usb_audio_transmitter_op(TRANSMITTER_USR_USB_AUDIO_MIX1, action);
                    }
#else
                    app_usb_audio_transmitter_op(TRANSMITTER_USR_USB_AUDIO_MIX1, action);
#endif
                }
#endif
#endif
#ifdef AIR_USB_AUDIO_OUT_ENABLE
                if (port & USB_AUDIO_OUT_PORT1) {
                    s_usr_group[TRANSMITTER_USR_USB_OUT].last_action = action;
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
                    if (action == USB_AUDIO_ACTION_OPEN) {
                        app_usb_out_request_source();
                    } else {
                        app_usb_audio_transmitter_op(TRANSMITTER_USR_USB_OUT, action);
                    }
#else
                    app_usb_audio_transmitter_op(TRANSMITTER_USR_USB_OUT, action);
#endif
                }
#endif
            }
            break;
        }
#if defined(AIR_USB_AUDIO_MIX_ENABLE) || defined(AIR_USB_AUDIO_OUT_ENABLE)
        case APPS_EVENTS_INTERACTION_USB_AUDIO_TRANSMITTER:
            app_usb_audio_transmitter_event(self, (uint16_t)(uint32_t)extra_data);
            break;
#endif
#if defined(AIR_USB_AUDIO_MIX_ENABLE) || defined(AIR_USB_AUDIO_OUT_ENABLE)
        case APPS_EVENTS_INTERACTION_USB_VOLUME_SET:
            __app_usb_audio_set_transmitter_volume((TRANSMITTER_USR_T)extra_data);
            break;
#endif
#if defined(AIR_USB_AUDIO_MIX_ENABLE) || defined(AIR_USB_AUDIO_OUT_ENABLE)
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
            case APPS_EVENTS_INTERACTION_USB_AUDIO_SRC_CTRL_EV: {
                uint32_t shell_event = (uint32_t)extra_data;
                app_usb_in_audio_source_event(shell_event);
                break;
            }
#endif
#endif
    }
    return false;
}

bool app_audio_vendor_event(ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
#if defined(AIR_USB_AUDIO_MIX_ENABLE) || defined(AIR_USB_AUDIO_OUT_ENABLE)
    vendor_se_event_t event = event_id;
    if (event == EVENT_RECORD_STOP) {
        USB_AUDIO_LOG_I("try to resume: %d after recorder", 0);
        uint32_t i = 0;
        for (i = 0; i < TRANSMITTER_USR_MAX; i++) {
            if (s_usr_group[i].suspend_by == AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_RECORDER ||
                s_usr_group[i].suspend_by == AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_RECORDER) {
                USB_AUDIO_LOG_I("try to resume: %d after recorder", 1, i);
                app_usb_audio_transmitter_op((TRANSMITTER_USR_T)i, USB_AUDIO_ACTION_OPEN);
            }
        }
    }
#ifdef AIR_USB_HID_MEDIA_CTRL_ENABLE
    if (event == EVENT_USB_AUDIO_START) {
        USB_AUDIO_LOG_I("start usb audio activity due to usb audio started", 0);
        ui_shell_start_activity(self, app_usb_audio_activity, ACTIVITY_PRIORITY_LOW, NULL, 0);
    }
#endif
#ifdef AIR_LE_AUDIO_ENABLE
    if (event == EVENT_BLE_STOP) {
        app_usb_resume_after_le_audio();
    }
#endif
#endif
    return ret;
}
