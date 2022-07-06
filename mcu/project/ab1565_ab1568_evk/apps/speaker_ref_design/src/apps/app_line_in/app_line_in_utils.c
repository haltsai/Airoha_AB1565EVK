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

#include "app_line_in_utils.h"
#ifdef AIR_LINE_OUT_ENABLE
#include "bt_sink_srv_ami.h"
#endif

#define DEFAULT_VOLUME 12

/*========================================================================================*/
/*                                  classcial line in                                     */
/*========================================================================================*/
#ifdef APPS_LINE_IN_SUPPORT
bool audio_sink_line_in_control_started = false;
static volatile bool need_resume = false;
static volatile bool hfp_started = false;

void app_line_in_start_stop(app_audio_path_t new_path)
{
    if (new_path == APP_AUDIO_PATH_LINE_IN) {
        /* Enable line-in on the hardware board. */
        if (!hfp_started) {
            hal_gpio_set_output(AUDIO_PATH_ANALOG_SW_PORT, AUDIO_PATH_ANALOG_LINE_IN);
        }

        /* Start line-in audio. */
        if (audio_sink_line_in_control_started == false) {
            audio_sink_srv_line_in_control_action_handler(AUDIO_SINK_SRV_LINE_IN_ACT_DEVICE_PLUG_IN, NULL);
            audio_sink_srv_line_in_control_action_handler(AUDIO_SINK_SRV_LINE_IN_ACT_TRIGGER_START, NULL);

            audio_sink_line_in_control_started = true;
        }
    } else if (new_path == APP_AUDIO_PATH_BT) {
        /* Disable line-in on the hardware board. */
        hal_gpio_set_output(AUDIO_PATH_ANALOG_SW_PORT, AUDIO_PATH_ANALOG_MIC_IN);

        /* Stop line-in audio. */
        if (audio_sink_line_in_control_started == true) {
            audio_sink_srv_line_in_control_action_handler(AUDIO_SINK_SRV_LINE_IN_ACT_TRIGGER_STOP, NULL);
            audio_sink_srv_line_in_control_action_handler(AUDIO_SINK_SRV_LINE_IN_ACT_DEVICE_PLUG_OUT, NULL);

            audio_sink_line_in_control_started = false;
        }
    } 
}

/*========================================================================================*/
/*                                  common function                                       */
/*========================================================================================*/
extern app_audio_path_t current_audio_path_value;

static void line_in_am_callback(vendor_se_event_t event, void *arg)
{
    LINE_IN_LOG_I("am callback, event: %d, current path: %d, started: %d", 3,
                  event, current_audio_path_value, audio_sink_line_in_control_started);
    ui_shell_send_event(true, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_SWITCH_AUDIO_PATH,
                        APPS_EVENTS_INTERACTION_AM_EVENT,
                        (void*)event, 0, NULL, 0);
}

void app_line_in_am_init()
{
    int32_t se_id = 0;
    bt_sink_srv_am_result_t ret;
    need_resume = false;

    se_id = ami_get_vendor_se_id();
    ret = ami_register_vendor_se(se_id, line_in_am_callback);
    LINE_IN_LOG_I("register vendor se ret:%d", 1, ret);
}

void app_line_in_handle_am_event(vendor_se_event_t event) {
    switch (event) {
        case EVENT_HFP_START:
            hfp_started = true;
            hal_gpio_set_output(AUDIO_PATH_ANALOG_SW_PORT, AUDIO_PATH_ANALOG_MIC_IN);
            break;
        case EVENT_LINEINPLAYBACK_SUSPEND:
        case EVENT_LINEINPLAYBACK_REJECT:
            app_line_in_switch(APP_AUDIO_PATH_BT);
            break;
        case EVENT_HFP_STOP:
            hfp_started = false;
            if (current_audio_path_value == APP_AUDIO_PATH_LINE_IN) {
                hal_gpio_set_output(AUDIO_PATH_ANALOG_SW_PORT, AUDIO_PATH_ANALOG_LINE_IN);
            }
            break;
        case EVENT_RECORD_STOP:
            if (current_audio_path_value == APP_AUDIO_PATH_LINE_IN) {
                app_line_in_switch(APP_AUDIO_PATH_LINE_IN);
            }
            break;
    }
}
#endif

/*========================================================================================*/
/*                                  common function                                       */
/*========================================================================================*/
#if defined(AIR_LINE_IN_MIX_ENABLE) || defined(AIR_LINE_OUT_ENABLE)
typedef enum {
    TRANSMITTER_USR_LINE_IN_MIX,
    TRANSMITTER_USR_LINE_OUT,
    TRANSMITTER_USR_MAX
} TRANSMITTER_USR_T;

typedef struct {
    TRANSMITTER_USR_T type;
    audio_transmitter_id_t transmitter_id;
    bool is_waitting_signal;
    app_audio_path_t pending_action;
    bool started;
    audio_transmitter_event_t suspend_by;
} transmitter_usr;

static transmitter_usr s_usr_group[TRANSMITTER_USR_MAX];
static uint8_t s_line_in_volume= DEFAULT_VOLUME;

#if 0
static uint8_t app_line_in_get_nvdm_volume() {
    uint8_t volume = 0;
    nvkey_status_t ret = nvkey_read_data(, &volume, sizeof(uint8_t));
    if (ret == NVKEY_STATUS_ITEM_NOT_FOUND) {
        volume = DEFAULT_VOLUME;
        ret = nvkey_write_data(, &volume, sizeof(uint8_t));
        LINE_IN_LOG_I("write initial volume to nvdm, ret:%d", 1, ret);
        return DEFAULT_VOLUME;
    } else if (ret != NVKEY_STATUS_OK) {
        LINE_IN_LOG_I("read volume failed, ret:%d", 1, ret);
        return DEFAULT_VOLUME;
    }

    return volume;
}
#endif

static void app_line_in_transmitter_event_callback(audio_transmitter_event_t event, void *data, void *user_data)
{
    transmitter_usr* ctx = (transmitter_usr*)user_data;
    uint32_t shell_event = ((ctx->type & 0xff) << 8) | (event & 0xff);
    LINE_IN_LOG_I("audio transmitter event, %d:%d", 2, ctx->type, event);
    ui_shell_send_event(true, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_SWITCH_AUDIO_PATH,
                        APPS_EVENTS_INTERACTION_LINE_IN_TRANSMITTER,
                        (void*)shell_event, 0, NULL, 0);
}

static void app_line_in_transmitter_init(TRANSMITTER_USR_T type, audio_transmitter_config_t *cfg)
{
    cfg->user_data = &s_usr_group[type];
    audio_transmitter_id_t id = audio_transmitter_init(cfg);
    LINE_IN_LOG_I("audio transmitter init, type:%d, re: %d", 2, type, id);
    s_usr_group[type].type = type;
    s_usr_group[type].transmitter_id = id;
    s_usr_group[type].is_waitting_signal = false;
    s_usr_group[type].pending_action = APP_AUDIO_PATH_UNKNOWN;
    s_usr_group[type].started = false;
    s_usr_group[type].suspend_by = AUDIO_TRANSMITTER_EVENT_START_SUCCESS;
}

static void app_line_in_transmitter_start_stop(TRANSMITTER_USR_T type, app_audio_path_t path) {
    audio_transmitter_status_t sta = AUDIO_TRANSMITTER_STATUS_FAIL;
    transmitter_usr* ctx = &s_usr_group[type];
    LINE_IN_LOG_I("audio transmitter start stop, type:%d, pd: %d, w: %d, p: %d, c:%d", 5,
        type, path, ctx->is_waitting_signal, ctx->pending_action, ctx->started);
    if (ctx->is_waitting_signal) {
        ctx->pending_action = path;
        return;
    }

    if (path == APP_AUDIO_PATH_LINE_IN) {
        if (ctx->started && ctx->suspend_by < AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_HFP) {
            return;
        }
        ctx->suspend_by = AUDIO_TRANSMITTER_EVENT_START_SUCCESS;
        sta = audio_transmitter_start(ctx->transmitter_id);
    } else if (path == APP_AUDIO_PATH_BT) {
        if (!ctx->started) {
            return;
        }
        if (ctx->suspend_by >= AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_HFP) {
            LINE_IN_LOG_I("line in already suspend by: %d", 1, ctx->suspend_by);
            ctx->started = false;
            ctx->pending_action = APP_AUDIO_PATH_UNKNOWN;
            return;
        }
        ctx->suspend_by = AUDIO_TRANSMITTER_EVENT_START_SUCCESS;
        sta = audio_transmitter_stop(ctx->transmitter_id);
    }

    LINE_IN_LOG_I("audio transmitter start stop, result: %d", 1, sta);
    if (sta == AUDIO_TRANSMITTER_STATUS_SUCCESS) {
        ctx->is_waitting_signal = true;
        ctx->started = path == APP_AUDIO_PATH_LINE_IN ? true : false;
        ctx->pending_action = APP_AUDIO_PATH_UNKNOWN;
    }
}

void app_line_in_transmitter_event(uint16_t event)
{
    TRANSMITTER_USR_T type = (event & 0xff00) >> 8;
    audio_transmitter_event_t ev = event & 0xff;
    transmitter_usr* ctx = &s_usr_group[type];

    LINE_IN_LOG_I("audio transmitter ev, type:%d, ev: %d, w: %d, p: %d, c: %d", 5, type, ev, ctx->is_waitting_signal,
        ctx->pending_action, ctx->started);
    switch(ev) {
        case AUDIO_TRANSMITTER_EVENT_START_SUCCESS:
            app_line_in_set_volume(s_line_in_volume);
        case AUDIO_TRANSMITTER_EVENT_START_FAIL:
        case AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS:
            if (ctx->is_waitting_signal) {
                ctx->is_waitting_signal = false;
                if (ctx->pending_action != APP_AUDIO_PATH_UNKNOWN) {
                    app_line_in_transmitter_start_stop(type, ctx->pending_action);
                    ctx->pending_action = APP_AUDIO_PATH_UNKNOWN;
                }
            }
            /* Enable or disable for line out */
            if (type == TRANSMITTER_USR_LINE_OUT) {
                bt_sink_srv_am_result_t ret = AUD_EXECUTION_SUCCESS;
                if (ev == AUDIO_TRANSMITTER_EVENT_START_SUCCESS) {
                    am_audio_side_tone_enable();
                } else if (ev == AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS) {
                    am_audio_side_tone_disable();
                }
                LINE_IN_LOG_I("enable or disable side tone for line out", 1, ret);
            }
            break;
        case AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_HFP:
        case AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_HFP:
        case AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_RECORDER:
        case AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_A2DP:
        case AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_A2DP:
        case AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_RECORDER:
        case AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_USB_IN:
        case AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_USB_OUT:
            if (ctx->is_waitting_signal) {
                ctx->is_waitting_signal = false;
            }

            /* If the close operation happend before reject or suspend, do not resume it. */
            if (ctx->started && ctx->pending_action != APP_AUDIO_PATH_BT) {
                ctx->suspend_by = ev;
            }

            /* If close operation in pending, ignore it. */
            if (ctx->pending_action == APP_AUDIO_PATH_BT) {
                ctx->pending_action = APP_AUDIO_PATH_UNKNOWN;
            }
            /* Check and disable side tone for line out. */
            if (type == TRANSMITTER_USR_LINE_OUT) {
                bt_sink_srv_am_result_t ret = am_audio_side_tone_disable();
                LINE_IN_LOG_I("enable side tone for line out", 1, ret);
            }
            break;
    }
}
#endif


void app_line_in_handle_sink_event(uint32_t event_id, void *extra_data, size_t data_len) {
#if defined(AIR_LINE_OUT_ENABLE)
    uint32_t i = 0;
    if (event_id == BT_SINK_SRV_EVENT_STATE_CHANGE) {
        bt_sink_srv_state_change_t *param = (bt_sink_srv_state_change_t *) extra_data;
        if ((param->previous >= BT_SINK_SRV_STATE_STREAMING) && (param->current <= BT_SINK_SRV_STATE_CONNECTED)) {
            LINE_IN_LOG_I("try to resume after hfp or a2dp", 0);
            for (i = 0; i < TRANSMITTER_USR_MAX; i++) {
                switch (s_usr_group[i].suspend_by) {
                    case AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_HFP:
                    case AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_HFP:
                    case AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_A2DP:
                    case AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_A2DP:
                        LINE_IN_LOG_I("resume: %d after suspend by:%d", 2, i, s_usr_group[i].suspend_by);
                        app_line_in_transmitter_start_stop((TRANSMITTER_USR_T)i, APP_AUDIO_PATH_LINE_IN);
                        break;
                }
            }
        }
    }
#endif
}

#if defined(AIR_LINE_IN_MIX_ENABLE)
uint8_t app_line_in_get_volume() {
    return s_line_in_volume;
}

void app_line_in_set_volume(uint8_t volume) {
    audio_transmitter_status_t sta = AUDIO_TRANSMITTER_STATUS_FAIL;
    transmitter_usr* ctx = &s_usr_group[TRANSMITTER_USR_LINE_IN_MIX];
    if (ctx->started) {
        audio_transmitter_runtime_config_t config;
        config.wired_audio_runtime_config.vol_level.vol_level_l= volume;
        sta = audio_transmitter_set_runtime_config(ctx->transmitter_id, WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_LINEIN, &config);
        LINE_IN_LOG_I("volume set result: %d", 1, sta);
        s_line_in_volume = volume;
    }
}
#endif

/*========================================================================================*/
/*                                     line in mix                                        */
/*========================================================================================*/
#if defined(AIR_LINE_IN_MIX_ENABLE)
void app_line_in_mix_init() {
    audio_transmitter_config_t config;
    config.scenario_type = AUDIO_TRANSMITTER_WIRED_AUDIO;
    config.scenario_sub_id = AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_IN;
    config.msg_handler = app_line_in_transmitter_event_callback;
    config.user_data = NULL;

    app_line_in_transmitter_init(TRANSMITTER_USR_LINE_IN_MIX, &config);
}

void app_line_in_mix_start_stop(app_audio_path_t path) {
    app_line_in_transmitter_start_stop(TRANSMITTER_USR_LINE_IN_MIX, path);
}
#endif


/*========================================================================================*/
/*                                      line out                                          */
/*========================================================================================*/
#if defined(AIR_LINE_OUT_ENABLE)
void app_line_out_init() {
    audio_transmitter_config_t config;
    config.scenario_type = AUDIO_TRANSMITTER_WIRED_AUDIO;
    config.scenario_sub_id = AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT;
    config.msg_handler = app_line_in_transmitter_event_callback;
    config.user_data = NULL;

    app_line_in_transmitter_init(TRANSMITTER_USR_LINE_OUT, &config);
}

void app_line_out_start_stop(app_audio_path_t path) {
    app_line_in_transmitter_start_stop(TRANSMITTER_USR_LINE_OUT, path);
}
#endif

