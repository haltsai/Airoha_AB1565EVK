/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
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

#include "app_anc_extend_gain_activity.h"

#include "apps_aws_sync_event.h"
#include "apps_debug.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "app_rho_idle_activity.h"

#include "app_anc_service.h"
#include "anc_monitor.h"
#include "bt_aws_mce_srv.h"
#include "bt_connection_manager.h"
#include "bt_device_manager.h"
#include "hal_audio.h"
#include "hal_audio_internal.h"
#include "hal_gpt.h"
#ifdef MTK_RACE_CMD_ENABLE
#include "race_cmd_dsprealtime.h"
#include "race_event.h"
#endif
#ifdef MTK_NVDM_ENABLE
#include "nvkey.h"
#include "nvkey_id_list.h"
#endif

#define APP_ANC_EXTEND_GAIN_TEST

#define LOG_TAG           "[anc_gain]"

#define APP_ANC_EXTEND_GAIN_SYNC_DELAY         (600)
/* UU status: 1 for loose, 0 for tight. */
#define APP_ANC_EXTEND_GAIN_UU_INIT            (0xFF)
#define APP_ANC_EXTEND_GAIN_UU_INIT_TIMEOUT    (2 * 1000)

#define APP_ANC_EXTEND_GAIN_ED_INVAILD         (0xFF)

#define APP_UU_ED_MODE_INIT  (0)
#define APP_UU_ED_MODE_UU    (1)
#define APP_UU_ED_MODE_ED    (2)

typedef struct {
    // wind_noise - wn, user_unaware - uu, environment_detection - ed
    uint8_t                  uu_ed_mode;

    audio_anc_control_gain_t wn_local;
    audio_anc_control_gain_t wn_peer;
    audio_anc_control_gain_t wn_apply;

    audio_anc_control_gain_t uu_local;
    audio_anc_control_gain_t uu_peer;
    audio_anc_control_gain_t uu_apply;              /* 1 - loose, 0 - tight. */

    audio_anc_control_gain_t ed_ff_local;
    audio_anc_control_gain_t ed_ff_peer;
    audio_anc_control_gain_t ed_ff_apply;
    audio_anc_control_gain_t ed_fb_local;
    audio_anc_control_gain_t ed_fb_peer;
    audio_anc_control_gain_t ed_fb_apply;
    uint8_t                  ed_level_local;
    uint8_t                  ed_level_peer;
    uint8_t                  ed_level_apply;
    int16_t                  ed_peer_stationary_noise;
} app_anc_extend_gain_context_t;

typedef struct {
    audio_anc_control_gain_t gain_1;
    audio_anc_control_gain_t gain_2;
    bt_clock_t               bt_clk;
} app_anc_extend_gain_sync_t;

typedef struct {
    audio_anc_control_gain_t ff_gain;
    audio_anc_control_gain_t fb_gain;
    uint8_t                  ed_level;
} app_anc_environment_detection_param_t;

typedef struct {
    uint8_t                  ed_level;
    int16_t                  stationary_noise;
} PACKED app_anc_environment_detection_aws_sync_t;

typedef struct {
    int16_t                  stationary_noise;
    bool                     is_response;
} PACKED app_anc_environment_detection_noise_sync_t;

typedef struct {
    uint8_t wn_enable : 1;
    uint8_t uu_enable : 1;
    uint8_t ed_enable : 1;
} PACKED app_anc_gain_control_t;

static app_anc_extend_gain_context_t        s_app_anc_gain_ctx = {0};
static app_anc_gain_control_t               s_app_anc_gain_control = {0};
extern bool                                 g_app_extend_gain_activity_started;

/* Currently Not use but need a pointer pass to DSP callback.*/
static uint32_t                             s_app_extend_gain_monitor = 0;

#ifdef APP_ANC_EXTEND_GAIN_TEST
#include "atci.h"

static atci_status_t app_anc_gain_atci_handler(atci_parse_cmd_param_t *parse_cmd);
static atci_cmd_hdlr_item_t app_anc_gain_atci_cmd[] = {
    {
        .command_head = "AT+ANCGAIN",
        .command_hdlr = app_anc_gain_atci_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
};
#endif

static bool app_anc_extend_gain_set_local(audio_anc_control_extend_ramp_gain_type_t type, audio_anc_control_gain_t gain_1,
                                          audio_anc_control_gain_t gain_2, uint32_t target_gpt);

#ifdef MTK_RACE_CMD_ENABLE

static void app_anc_extend_gain_notify_uu_result(bool is_response)
{
#ifdef AIR_ANC_USER_UNAWARE_ENABLE
    APPS_LOG_MSGID_I(LOG_TAG" notify_uu_result, is_response=%d uu_gain=%d",
                     2, is_response, s_app_anc_gain_ctx.uu_apply);
    if (is_response) {
        race_dsprealtime_anc_user_unaware_response(s_app_anc_gain_ctx.uu_apply);
    } else {
        race_dsprealtime_anc_user_unaware_notify(s_app_anc_gain_ctx.uu_apply);
    }
#else
    race_dsprealtime_anc_notify_gain_error(RACE_ANC_GAIN_STATUS_FEATURE_NOT_ENABLE,
                                           RACE_EVENT_TYPE_ANC_GAIN_USER_UNAWARE);
#endif
}

static void app_anc_extend_gain_notify_wn_result(bool is_response)
{
#ifdef AIR_ANC_WIND_DETECTION_ENABLE
    APPS_LOG_MSGID_I(LOG_TAG" notify_wn_result, is_response=%d wn_gain=%d",
                     2, is_response, s_app_anc_gain_ctx.wn_apply);
    if (is_response) {
        race_dsprealtime_anc_wind_noise_response(s_app_anc_gain_ctx.wn_apply);
    } else {
        race_dsprealtime_anc_wind_noise_notify(s_app_anc_gain_ctx.wn_apply);
    }
#else
    race_dsprealtime_anc_notify_gain_error(RACE_ANC_GAIN_STATUS_FEATURE_NOT_ENABLE,
                                           RACE_EVENT_TYPE_ANC_GAIN_WIND_NOISE);
#endif
}

static void app_anc_extend_gain_notify_ed_result_imp(bool is_response)
{
#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
    int16_t stationary_noise = audio_environment_detection_get_stationary_noise();
    APPS_LOG_MSGID_I(LOG_TAG" notify_ed_result, is_response=%d ff_gain=%d fb_gain=%d level=%d stationary_noise=%d/%d",
                     6, is_response, s_app_anc_gain_ctx.ed_ff_apply, s_app_anc_gain_ctx.ed_fb_apply,
                     s_app_anc_gain_ctx.ed_level_apply, stationary_noise, s_app_anc_gain_ctx.ed_peer_stationary_noise);

    audio_anc_control_gain_t ed_ff_apply = s_app_anc_gain_ctx.ed_ff_apply;
    audio_anc_control_gain_t ed_fb_apply = s_app_anc_gain_ctx.ed_fb_apply;
    uint8_t                  ed_level_apply = s_app_anc_gain_ctx.ed_level_apply;

    // When UU loose, all ED gain/level should be invalid/disable status.
    if (s_app_anc_gain_ctx.uu_apply == 1) {
        ed_ff_apply = 0;
        ed_fb_apply = 0;
        ed_level_apply = 0;
        APPS_LOG_MSGID_I(LOG_TAG" notify_ed_result, zero ED when UU loose", 0);
    }

    if (is_response) {
        race_dsprealtime_anc_environment_detection_response(ed_level_apply,
                                                            ed_ff_apply,
                                                            ed_fb_apply,
                                                            stationary_noise,
                                                            s_app_anc_gain_ctx.ed_peer_stationary_noise);
    } else {
        race_dsprealtime_anc_environment_detection_notify(ed_level_apply,
                                                          ed_ff_apply,
                                                          ed_fb_apply,
                                                          stationary_noise,
                                                          s_app_anc_gain_ctx.ed_peer_stationary_noise);
    }
#endif
}

static void app_anc_extend_gain_notify_ed_noise(bool is_response)
{
#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    int16_t stationary_noise = audio_environment_detection_get_stationary_noise();
    app_anc_environment_detection_noise_sync_t noise_sync = {0};
    noise_sync.stationary_noise = stationary_noise;
    noise_sync.is_response = is_response;

    apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN,
                                   APP_EXTEND_GAIN_ED_NOTIFY_ED_NOISE,
                                   (void *)&noise_sync, sizeof(app_anc_environment_detection_noise_sync_t));
    APPS_LOG_MSGID_I(LOG_TAG" notify_ed_noise, [%02X] stationary_noise=%d is_response=%d",
                     3, role, stationary_noise, is_response);
#endif
}

static void app_anc_extend_gain_notify_ed_result(bool is_response)
{
#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
    if (BT_AWS_MCE_SRV_LINK_NONE != bt_aws_mce_srv_get_link_type()) {
        app_anc_extend_gain_notify_ed_noise(is_response);
        return;
    }

    app_anc_extend_gain_notify_ed_result_imp(is_response);
#else
    race_dsprealtime_anc_notify_gain_error(RACE_ANC_GAIN_STATUS_FEATURE_NOT_ENABLE,
                                           RACE_EVENT_TYPE_ANC_GAIN_ENVIRONMENT_DETECTION);
#endif
}

void app_anc_extend_gain_init_control_status()
{
#ifdef MTK_NVDM_ENABLE
    uint32_t size = sizeof(app_anc_gain_control_t);
    nvkey_status_t status = nvkey_read_data(NVKEYID_APP_ANC_GAIN_CONTROL_STATUS, (uint8_t *)&s_app_anc_gain_control, &size);
    APPS_LOG_MSGID_I(LOG_TAG" init_control_status, read status=%d", 1, status);
    if (status == NVKEY_STATUS_ITEM_NOT_FOUND) {
#ifdef AIR_ANC_WIND_DETECTION_ENABLE
        s_app_anc_gain_control.wn_enable = 1;
#else
        s_app_anc_gain_control.wn_enable = 0;
#endif
#ifdef AIR_ANC_USER_UNAWARE_ENABLE
        s_app_anc_gain_control.uu_enable = 1;
#else
        s_app_anc_gain_control.uu_enable = 0;
#endif
#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
        s_app_anc_gain_control.ed_enable = 1;
#else
        s_app_anc_gain_control.ed_enable = 0;
#endif
        size = sizeof(app_anc_gain_control_t);
        status = nvkey_write_data(NVKEYID_APP_ANC_GAIN_CONTROL_STATUS, (const uint8_t *)&s_app_anc_gain_control, size);
        APPS_LOG_MSGID_I(LOG_TAG" init_control_status, write status=%d", 1, status);
    } else if (status != NVKEY_STATUS_OK) {
        APPS_LOG_MSGID_E(LOG_TAG" init_control, nvdm read fail %d", 1, status);
    }
#endif

    APPS_LOG_MSGID_I(LOG_TAG" init_control, wd_enable=%d, uu_enable=%d, ed_enable=%d", 3,
                    s_app_anc_gain_control.wn_enable, s_app_anc_gain_control.uu_enable, s_app_anc_gain_control.ed_enable);
    if (!s_app_anc_gain_control.wn_enable) {
        /* Cannot enable UU when WN disabled. */
        s_app_anc_gain_control.uu_enable = 0;
    }

#ifdef AIR_ANC_WIND_DETECTION_ENABLE
        if (!s_app_anc_gain_control.wn_enable) {
            /* Disable wind noise detection. */
            audio_wind_detection_enable(false);
        } else {
            audio_wind_detection_enable(true);
        }
#endif
#ifdef AIR_ANC_USER_UNAWARE_ENABLE
        if (!s_app_anc_gain_control.uu_enable) {
            audio_user_unaware_enable(false);
            s_app_anc_gain_ctx.uu_local = APP_ANC_EXTEND_GAIN_UU_INIT;
            s_app_anc_gain_ctx.uu_peer = APP_ANC_EXTEND_GAIN_UU_INIT;
            s_app_anc_gain_ctx.uu_apply = APP_ANC_EXTEND_GAIN_UU_INIT;
            s_app_anc_gain_ctx.uu_ed_mode = APP_UU_ED_MODE_ED;
        }
#endif
#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
        if (!s_app_anc_gain_control.ed_enable) {
            audio_environment_detection_suspend_procedure(true);
            s_app_anc_gain_ctx.uu_ed_mode = APP_UU_ED_MODE_INIT;
        }
#endif

    APPS_LOG_MSGID_I(LOG_TAG" init_control_status, control=0x%02X", 1, s_app_anc_gain_control);
}

#if (defined AIR_ANC_USER_UNAWARE_ENABLE) || (defined AIR_ANC_ENVIRONMENT_DETECTION_ENABLE) || (defined AIR_ANC_WIND_DETECTION_ENABLE)
static void app_anc_extend_gain_save_control_status()
{
#ifdef MTK_NVDM_ENABLE
    uint32_t size = sizeof(app_anc_gain_control_t);
    nvkey_status_t status = nvkey_write_data(NVKEYID_APP_ANC_GAIN_CONTROL_STATUS,
                                             (const uint8_t *)&s_app_anc_gain_control, size);
    APPS_LOG_MSGID_I(LOG_TAG" save_control_status, control=0x%02X status=%d",
                     2, s_app_anc_gain_control, status);
#endif
}
#endif

static void app_anc_extend_gain_control(anc_gain_control_param_t *param)
{
    bool status = RACE_ANC_GAIN_STATUS_UNKNOWN_ERROR;
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    uint8_t type = param->type;
    uint8_t action = param->action;
    APPS_LOG_MSGID_I(LOG_TAG" gain_control, [%02X] type=%d action=%d", 3, role, type, action);

    // Check Feature option
    bool feature_enable = FALSE;
    if (type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_WIND_NOISE) {
#ifdef AIR_ANC_WIND_DETECTION_ENABLE
        feature_enable = TRUE;
#endif
    } else if (type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_USER_UNAWARE) {
#ifdef AIR_ANC_USER_UNAWARE_ENABLE
        feature_enable = TRUE;
#endif
    } else if (type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION) {
#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
        feature_enable = TRUE;
#endif
    } else {
        APPS_LOG_MSGID_E(LOG_TAG" gain_control, invalid type", 0);
        return;
    }

    if (!feature_enable) {
        APPS_LOG_MSGID_E(LOG_TAG" gain_control, error feature_not_enable", 0);
        race_dsprealtime_anc_gain_control_response(RACE_ANC_GAIN_STATUS_FEATURE_NOT_ENABLE, type, FALSE);
        return;
    }

    // Query action, only for Agent, not request AWS connected
    if (role == BT_AWS_MCE_ROLE_AGENT
        && action == RACE_ANC_GAIN_CONTROL_QUERY) {
        bool enable = FALSE;
        if (type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_WIND_NOISE) {
            enable = s_app_anc_gain_control.wn_enable;
        } else if (type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_USER_UNAWARE) {
            enable = s_app_anc_gain_control.uu_enable;
        } else if (type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION) {
            enable = s_app_anc_gain_control.ed_enable;
        }
        APPS_LOG_MSGID_I(LOG_TAG" gain_control, [%02X] QUERY type=%d enable=%d", 3, role, type, enable);
        race_dsprealtime_anc_gain_control_response(RACE_ANC_GAIN_STATUS_SUCCESS, type, enable);
        return;
    }

    // Enable/Disable action must be AWS connected.
    if (role == BT_AWS_MCE_ROLE_AGENT
            && BT_AWS_MCE_SRV_LINK_NONE == bt_aws_mce_srv_get_link_type()) {
        APPS_LOG_MSGID_E(LOG_TAG" gain_control, Agent not AWS Connected", 0);
        race_dsprealtime_anc_gain_control_response(RACE_ANC_GAIN_STATUS_AWS_NOT_CONNTECT, type, FALSE);
        return;
    }

    // Enable/Disable action for Agent/Partner
    switch (type) {
        case AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_WIND_NOISE: {
#ifdef AIR_ANC_WIND_DETECTION_ENABLE
            // Need to enable/disable Wind Detection
            bool wn_enable = (action == RACE_ANC_GAIN_CONTROL_DISABLE ? FALSE : TRUE);
            if (wn_enable == s_app_anc_gain_control.wn_enable) {
                APPS_LOG_MSGID_I(LOG_TAG" gain_control, [%02X] WIND_NOISE, already on state %d", 2, role, wn_enable);
                status = RACE_ANC_GAIN_STATUS_SUCCESS;
                break;
            }
            APPS_LOG_MSGID_I(LOG_TAG" gain_control, [%02X] WIND_NOISE, enable? %d, uu %d", 3, role, wn_enable, s_app_anc_gain_control.uu_enable);
            s_app_anc_gain_control.wn_enable = wn_enable;
            app_anc_extend_gain_save_control_status();

            if (!wn_enable && s_app_anc_gain_control.uu_enable) {
                /* Should disable UU when disable wind noise. */
                s_app_anc_gain_control.uu_enable = false;
#ifdef AIR_ANC_USER_UNAWARE_ENABLE
                audio_user_unaware_enable(false);
                s_app_anc_gain_ctx.uu_local = APP_ANC_EXTEND_GAIN_UU_INIT;
                s_app_anc_gain_ctx.uu_peer = APP_ANC_EXTEND_GAIN_UU_INIT;
                s_app_anc_gain_ctx.uu_apply = APP_ANC_EXTEND_GAIN_UU_INIT;
                s_app_anc_gain_ctx.uu_ed_mode = APP_UU_ED_MODE_ED;
#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
                app_anc_extend_gain_set_local(AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION,
                        s_app_anc_gain_ctx.ed_ff_apply, s_app_anc_gain_ctx.ed_fb_apply, 0);
#endif
#endif
            }

            audio_wind_detection_enable(wn_enable);
            if (!wn_enable) {
                s_app_anc_gain_ctx.wn_local = 0;
                s_app_anc_gain_ctx.wn_peer = 0;
                s_app_anc_gain_ctx.wn_apply = 0;
            }
            status = RACE_ANC_GAIN_STATUS_SUCCESS;
#endif
            break;
        }
        case AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_HOWLING_CONTROL: {
            // Not use for now
            break;
        }
#ifdef AIR_ANC_USER_UNAWARE_ENABLE
        case AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_USER_UNAWARE: {
            bool uu_enable = (action == RACE_ANC_GAIN_CONTROL_DISABLE ? FALSE : TRUE);
            if (uu_enable == s_app_anc_gain_control.uu_enable) {
                APPS_LOG_MSGID_I(LOG_TAG" gain_control, [%02X] USER_UNAWARE, already on state %d", 2, role, uu_enable);
                status = RACE_ANC_GAIN_STATUS_SUCCESS;
                break;
            }

            if (!s_app_anc_gain_control.wn_enable && uu_enable) {
                /* Cannot enable UU when WN disabled. */
                APPS_LOG_MSGID_W(LOG_TAG" gain_control, [%02X] USER_UNAWARE, cannot enable since WN disabled", 1, role);
                status = RACE_ANC_GAIN_STATUS_UNKNOWN_ERROR;
                break;
            }

            audio_user_unaware_save_enable_status_to_nvdm(uu_enable);
#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
            if (uu_enable && s_app_anc_gain_control.ed_enable) {
                /* Set ED as 0 to ensure UU work. */
                app_anc_extend_gain_set_local(AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION, 0, 0, 0);
            }
#endif
            audio_user_unaware_enable(uu_enable);
            if (uu_enable) {
                /*
                ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN,
                                    APP_EXTEND_GAIN_USER_UNAWARE_INIT,
                                    NULL, 0, NULL, APP_ANC_EXTEND_GAIN_UU_INIT_TIMEOUT);
                */
                s_app_anc_gain_ctx.uu_ed_mode = APP_UU_ED_MODE_INIT;
            } else {
                s_app_anc_gain_ctx.uu_local = APP_ANC_EXTEND_GAIN_UU_INIT;
                s_app_anc_gain_ctx.uu_peer = APP_ANC_EXTEND_GAIN_UU_INIT;
                s_app_anc_gain_ctx.uu_apply = APP_ANC_EXTEND_GAIN_UU_INIT;
                s_app_anc_gain_ctx.uu_ed_mode = APP_UU_ED_MODE_ED;

                 app_anc_extend_gain_set_local(AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION,
                         s_app_anc_gain_ctx.ed_ff_apply, s_app_anc_gain_ctx.ed_fb_apply, 0);
            }

            status = RACE_ANC_GAIN_STATUS_SUCCESS;
            s_app_anc_gain_control.uu_enable = (action == RACE_ANC_GAIN_CONTROL_ENABLE);
            app_anc_extend_gain_save_control_status();
            APPS_LOG_MSGID_I(LOG_TAG" gain_control, [%02X] USER_UNAWARE enable=%d", 2, role, uu_enable);
            break;
        }
#endif
#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
        case AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION: {
            bool suspend = (action == RACE_ANC_GAIN_CONTROL_DISABLE ? TRUE :  FALSE);
            if (suspend != s_app_anc_gain_control.ed_enable) {
                APPS_LOG_MSGID_I(LOG_TAG" gain_control, [%02X] ENVIRONMENT_DETECTION, already on state %d", 2, role, s_app_anc_gain_control.ed_enable);
                status = RACE_ANC_GAIN_STATUS_SUCCESS;
                break;
            }
            if (suspend) {
                audio_anc_control_extend_ramp_cap_t anc_extend_ramp_cap = {0};
                anc_extend_ramp_cap.extend_gain_1 = 0;
                anc_extend_ramp_cap.extend_gain_2 = 0;
                audio_anc_control_result_t anc_ret = audio_anc_control_set_extend_gain(AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION,
                                                                                       &anc_extend_ramp_cap, NULL);
                APPS_LOG_MSGID_I(LOG_TAG" gain_control, [%02X] ENVIRONMENT_DETECTION clear gain anc_ret=%d when suspend",
                                 2, role, anc_ret);
#ifdef AIR_ANC_USER_UNAWARE_ENABLE
                if (s_app_anc_gain_ctx.uu_ed_mode == APP_UU_ED_MODE_ED && s_app_anc_gain_control.uu_enable) {
                    APPS_LOG_MSGID_I(LOG_TAG" gain_control, [%02X], enable UU", 0);
                    s_app_anc_gain_ctx.uu_ed_mode = APP_UU_ED_MODE_INIT;
                    audio_user_unaware_enable(TRUE);
                }
#endif
            } else {
                if (!s_app_anc_gain_control.uu_enable) {
                    s_app_anc_gain_ctx.uu_ed_mode = APP_UU_ED_MODE_ED;
                }
            }
            audio_environment_detection_suspend_procedure(suspend);
            status = RACE_ANC_GAIN_STATUS_SUCCESS;
            s_app_anc_gain_control.ed_enable = (action == RACE_ANC_GAIN_CONTROL_ENABLE);
            app_anc_extend_gain_save_control_status();
            APPS_LOG_MSGID_I(LOG_TAG" gain_control, [%02X] ENVIRONMENT_DETECTION suspend=%d", 2, role, suspend);
            break;
        }
#endif
    }

    // Sync to partner && reply to UT APP
    if (role == BT_AWS_MCE_ROLE_AGENT) {
        apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN_RACE,
                                       RACE_EVENT_TYPE_ANC_GAIN_CONTROL,
                                       (void *)param, sizeof(anc_gain_control_param_t));
        race_dsprealtime_anc_gain_control_response(status, type, (action == RACE_ANC_GAIN_CONTROL_ENABLE));
    }
}


void app_anc_extend_gain_query_status(void *param)
{
    anc_gain_control_param_t *control_param = (anc_gain_control_param_t *)param;
    if (control_param == NULL
        || control_param->action != RACE_ANC_GAIN_CONTROL_QUERY) {
        APPS_LOG_MSGID_E(LOG_TAG" query_status, error", 0);
        return;
    }
    app_anc_extend_gain_control(control_param);
}

#endif

static void app_anc_extend_gain_handle_callback(audio_anc_control_extend_ramp_gain_type_t type, audio_anc_control_gain_t gain_1,
                                                audio_anc_control_gain_t gain_2, uint8_t ed_level)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    APPS_LOG_MSGID_I(LOG_TAG" gain_handle_callback, [%02X] uu=%d/%d/%d wn=%d/%d/%d ed=%d/%d/%d %d/%d/%d %d/%d/%d",
                     16, role,
                     s_app_anc_gain_ctx.uu_local, s_app_anc_gain_ctx.uu_peer, s_app_anc_gain_ctx.uu_apply,
                     s_app_anc_gain_ctx.wn_local, s_app_anc_gain_ctx.wn_peer, s_app_anc_gain_ctx.wn_apply,
                     s_app_anc_gain_ctx.ed_ff_local, s_app_anc_gain_ctx.ed_ff_peer, s_app_anc_gain_ctx.ed_ff_apply,
                     s_app_anc_gain_ctx.ed_fb_local, s_app_anc_gain_ctx.ed_fb_peer, s_app_anc_gain_ctx.ed_fb_apply,
                     s_app_anc_gain_ctx.ed_level_local, s_app_anc_gain_ctx.ed_level_peer, s_app_anc_gain_ctx.ed_level_apply);

    uint32_t local_gain = gain_1;
    if (type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_WIND_NOISE) {
        APPS_LOG_MSGID_I(LOG_TAG" gain_handle_callback, [%02X] WIND_NOISE gain_1=%d", 2, role, gain_1);
        ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN,
                            APP_EXTEND_GAIN_WIND_NOISE_CHANGE,
                            (void *)local_gain, 0, NULL, 0);
    } else if (type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_USER_UNAWARE) {
        APPS_LOG_MSGID_I(LOG_TAG" gain_handle_callback, [%02X] USER_UNAWARE gain_1=%d", 2, role, gain_1);
        //ui_shell_remove_event(EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN, APP_EXTEND_GAIN_USER_UNAWARE_INIT);
        ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN,
                            APP_EXTEND_GAIN_USER_UNAWARE_CHANGE,
                            (void *)local_gain, 0, NULL, 0);
    } else if (type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION) {
        APPS_LOG_MSGID_I(LOG_TAG" gain_handle_callback, [%02X] ENVIRONMENT_DETECTION gain_1=%d gain_2=%d ed_level=%d",
                         4, role, gain_1, gain_2, ed_level);
        app_anc_environment_detection_param_t *environment_detection_param = (app_anc_environment_detection_param_t *)pvPortMalloc(sizeof(app_anc_environment_detection_param_t));
        if (environment_detection_param != NULL) {
            memset(environment_detection_param, 0, sizeof(app_anc_environment_detection_param_t));
            environment_detection_param->ff_gain = gain_1;
            environment_detection_param->fb_gain = gain_2;
            environment_detection_param->ed_level = ed_level;
            ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN,
                                APP_EXTEND_GAIN_ENVIRONMENT_DETECTION_CHANGE,
                                (void *)environment_detection_param, sizeof(app_anc_environment_detection_param_t), NULL, 0);
        } else {
            APPS_LOG_MSGID_E(LOG_TAG" gain_handle_callback, ENVIRONMENT_DETECTION malloc fail", 0);
        }
    }
}

static void app_anc_extend_gain_handle_ed_level(bool own, uint8_t ed_level)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    APPS_LOG_MSGID_I(LOG_TAG" [%02X] APP handle_ed_level own=%d ed_level=%d/%d/%d - %d",
                     6, role, own, s_app_anc_gain_ctx.ed_level_local, s_app_anc_gain_ctx.ed_level_peer,
                     s_app_anc_gain_ctx.ed_level_apply, ed_level);
    if (own) {
        s_app_anc_gain_ctx.ed_level_local = ed_level;
        // only send ENVIRONMENT_DETECTION_AWS_SYNC to peer after local updated
        app_anc_environment_detection_aws_sync_t aws_sync = {0};
        aws_sync.ed_level = ed_level;
#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
        aws_sync.stationary_noise = audio_environment_detection_get_stationary_noise();
#endif
        apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN,
                                       APP_EXTEND_GAIN_ENVIRONMENT_DETECTION_AWS_SYNC,
                                       (void *)&aws_sync, sizeof(app_anc_environment_detection_aws_sync_t));
    } else {
        s_app_anc_gain_ctx.ed_level_peer = ed_level;
    }

    // Disable if local or peer level == 0
    uint8_t ed_level_apply = 0;
    if (s_app_anc_gain_ctx.ed_level_local != 0 && s_app_anc_gain_ctx.ed_level_peer != 0) {
        ed_level_apply = (s_app_anc_gain_ctx.ed_level_local > s_app_anc_gain_ctx.ed_level_peer ? s_app_anc_gain_ctx.ed_level_local : s_app_anc_gain_ctx.ed_level_peer);
    }
    if (s_app_anc_gain_ctx.ed_level_apply != ed_level_apply) {
        s_app_anc_gain_ctx.ed_level_apply = ed_level_apply;
        APPS_LOG_MSGID_I(LOG_TAG" [%02X] APP handle_ed_level new ed_level=%d", 2, role, ed_level_apply);

        // only Agent notify to phone APP
        if (role == BT_AWS_MCE_ROLE_AGENT) {
            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN, APP_EXTEND_GAIN_NOTIFY_ENVIRONMENT_DETECTION_TO_PHONE_APP);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN,
                                APP_EXTEND_GAIN_NOTIFY_ENVIRONMENT_DETECTION_TO_PHONE_APP,
                                NULL, 0, NULL, 500);
        }
    }
}

static void app_anc_extend_gain_dsp_callback(hal_audio_event_t event, void *user_data)
{
    bool is_sync_control = false;
    audio_anc_control_result_t anc_control_result = 0;
    audio_anc_control_extend_ramp_cap_t anc_extend_ramp_cap;

    anc_extend_ramp_cap.gain_type = ((uint8_t)event);
    anc_extend_ramp_cap.extend_gain_1 = (*(uint32_t *)user_data) & 0xFFFF;
    anc_extend_ramp_cap.extend_gain_2 = ((*(uint32_t *)user_data) >> 16) & 0xFFFF;
    if (anc_extend_ramp_cap.gain_type ==  AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_WIND_NOISE
        || anc_extend_ramp_cap.gain_type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_USER_UNAWARE
        || anc_extend_ramp_cap.gain_type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION) {
        is_sync_control = true;
    } else if (anc_extend_ramp_cap.gain_type ==  AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_HOWLING_CONTROL) {
        is_sync_control = false;
    }
    /* Handle Noise Gate level. */
    uint8_t ed_level = 0;
    if (anc_extend_ramp_cap.gain_type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION) {
        ed_level = (uint8_t)((s_app_extend_gain_monitor >> 8) & 0xFF);
    }

    APPS_LOG_MSGID_I(LOG_TAG" gain_dsp_callback, type=%d gain1=%d gain2=%d ed_level=%d",
                     4, anc_extend_ramp_cap.gain_type, anc_extend_ramp_cap.extend_gain_1,
                     anc_extend_ramp_cap.extend_gain_2, ed_level);

    if (is_sync_control) {
        /* Notify APP. */
        app_anc_extend_gain_handle_callback(anc_extend_ramp_cap.gain_type,
                                            anc_extend_ramp_cap.extend_gain_1,
                                            anc_extend_ramp_cap.extend_gain_2,
                                            ed_level);
    } else {
        /* ANC control. */
        anc_control_result = audio_anc_control_set_extend_gain(anc_extend_ramp_cap.gain_type, &anc_extend_ramp_cap, NULL);
        APPS_LOG_MSGID_I(LOG_TAG" gain_dsp_callback, HOWLING_CONTROL result %d", 1, anc_control_result);
    }
}

static bool app_anc_extend_gain_set_local(audio_anc_control_extend_ramp_gain_type_t type, audio_anc_control_gain_t gain_1,
                                          audio_anc_control_gain_t gain_2, uint32_t target_gpt)
{
    bool success = false;
    audio_anc_control_extend_ramp_cap_t    anc_extend_ramp_cap;
    audio_anc_control_misc_t misc = {0};
    uint32_t cur_gpt = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &cur_gpt);
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();

    APPS_LOG_MSGID_I(LOG_TAG" set_local, [%02X] type=%d gain_1=%d gain_2=%d cur_gpt=%d target_gpt=%d",
                     6, role, type, gain_1, gain_2, cur_gpt, target_gpt);

    if (target_gpt != 0 && (target_gpt < cur_gpt || (target_gpt - cur_gpt) > APP_ANC_EXTEND_GAIN_SYNC_DELAY * 1000)) {
        APPS_LOG_MSGID_I(LOG_TAG" set_local, target_gpt set to 0", 0);
        target_gpt = 0;
    }

    if (type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_USER_UNAWARE) {
        /* User Unaware. */
        uint32_t delay_ms = 0;
        uint32_t gain_32 = (uint32_t)gain_1;

        if (gain_32 == APP_ANC_EXTEND_GAIN_UU_INIT || s_app_anc_gain_ctx.uu_local == APP_ANC_EXTEND_GAIN_UU_INIT) {
            return true;
        }
        if (target_gpt != 0) {
            delay_ms = (target_gpt - cur_gpt) / 1000;
        }
        ui_shell_send_event(false, EVENT_PRIORITY_HIGH,
                            EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN, APP_EXTEND_GAIN_USER_UNAWARE_SET,
                            (void *)gain_32, 0, NULL, delay_ms);
        success = true;
    } else if (type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_WIND_NOISE
            || type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION) {
        /* Wind Noise or Noise Gate. */
        misc.target_gpt_count = target_gpt;
        anc_extend_ramp_cap.extend_gain_1 = gain_1;
        anc_extend_ramp_cap.extend_gain_2 = gain_2;
        audio_anc_control_result_t anc_ret = AUDIO_ANC_CONTROL_EXECUTION_FAIL;
#if (defined AIR_ANC_USER_UNAWARE_ENABLE) && (defined AIR_ANC_ENVIRONMENT_DETECTION_ENABLE)
        if (type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION &&
            ((s_app_anc_gain_ctx.uu_ed_mode == APP_UU_ED_MODE_UU) ||
            (s_app_anc_gain_ctx.uu_ed_mode == APP_UU_ED_MODE_INIT && s_app_anc_gain_ctx.uu_local == APP_ANC_EXTEND_GAIN_UU_INIT))) {
            /* If UU is loose, cannot enable ENVIRONMENT_DETECTION. */
            APPS_LOG_MSGID_I(LOG_TAG" in UU loose/init, ignore ENVIRONMENT_DETECTION", 0);
        } else
#endif
        {
            anc_ret = audio_anc_control_set_extend_gain(type, &anc_extend_ramp_cap, &misc);
        }
        if (type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_WIND_NOISE) {
            APPS_LOG_MSGID_I(LOG_TAG" set_local, WIND_NOISE ret=%d", 1, anc_ret);
        } else {
            APPS_LOG_MSGID_I(LOG_TAG" set_local, ENVIRONMENT_DETECTION ret=%d", 1, anc_ret);
        }
        success = (AUDIO_ANC_CONTROL_EXECUTION_SUCCESS == anc_ret);

        if (success && type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION
            && role == BT_AWS_MCE_ROLE_AGENT) {
            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN, APP_EXTEND_GAIN_NOTIFY_ENVIRONMENT_DETECTION_TO_PHONE_APP);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN,
                                APP_EXTEND_GAIN_NOTIFY_ENVIRONMENT_DETECTION_TO_PHONE_APP,
                                NULL, 0, NULL, 500);
        }
    }
    return success;
}

static bool app_anc_extend_gain_set_and_sync(audio_anc_control_extend_ramp_gain_type_t type, audio_anc_control_gain_t gain_1,
                                             audio_anc_control_gain_t gain_2, uint32_t delay_ms)
{
    bt_clock_t bt_clk = {0};
    if (bt_sink_srv_bt_clock_addition(&bt_clk, 0, delay_ms * 1000) != BT_STATUS_SUCCESS) {
        APPS_LOG_MSGID_E(LOG_TAG" set_and_sync, get bt_clk fail", 0);
        return false;
    }
    APPS_LOG_MSGID_I(LOG_TAG" set_and_sync, type=%d gain_1=%d gain_2=%d delay_ms=%d",
                     4, type, gain_1, gain_2, delay_ms);

    app_anc_extend_gain_sync_t extra = {0};
    extra.gain_1 = gain_1;
    extra.gain_2 = gain_2;
    memcpy(&(extra.bt_clk), &bt_clk, sizeof(bt_clock_t));

    app_anc_extend_gain_event_t event = 0;
    if (type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_WIND_NOISE) {
        event = APP_EXTEND_GAIN_WIND_NOISE_CHANGE;
    } else if (type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_USER_UNAWARE) {
        if (gain_1 == APP_ANC_EXTEND_GAIN_UU_INIT) {
            return true;
        } else {
            event = APP_EXTEND_GAIN_USER_UNAWARE_CHANGE;
        }
    } else if (type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION) {
        event = APP_EXTEND_GAIN_ENVIRONMENT_DETECTION_CHANGE;
    }

    if (apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN, event,
                                       (void *)&extra, sizeof(app_anc_extend_gain_sync_t)) == BT_STATUS_SUCCESS) {
        uint32_t local_gpt = 0;
        if (bt_sink_srv_convert_bt_clock_2_gpt_count((const bt_clock_t *) & (extra.bt_clk), (uint32_t *)&local_gpt) == BT_STATUS_SUCCESS) {
            app_anc_extend_gain_set_local(type, gain_1, gain_2, local_gpt);
            return true;
        } else {
            APPS_LOG_MSGID_E(LOG_TAG" set_and_sync, bt_clk to gpt fail", 0);
        }
    } else {
        APPS_LOG_MSGID_E(LOG_TAG" set_and_sync, sync gain fail", 0);
    }
    return false;
}

static void app_anc_extend_gain_partner_report()
{
    app_anc_extend_gain_sync_t extra = {0};
    int16_t stationary_noise = 0;

#ifdef AIR_ANC_WIND_DETECTION_ENABLE
    /* Sync wind noise. */
    extra.gain_1 = s_app_anc_gain_ctx.wn_local;
    apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN, APP_EXTEND_GAIN_WIND_NOISE_CHANGE,
                                   (void *)&extra, sizeof(app_anc_extend_gain_sync_t));
#endif
#ifdef AIR_ANC_USER_UNAWARE_ENABLE
    /* Sync User Unaware. */
    extra.gain_1 = s_app_anc_gain_ctx.uu_local;
    apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN, APP_EXTEND_GAIN_USER_UNAWARE_CHANGE,
                                   (void *)&extra, sizeof(app_anc_extend_gain_sync_t));
#endif
#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
    /* Sync Environment Detection. */
    extra.gain_1 = s_app_anc_gain_ctx.ed_ff_local;
    extra.gain_2 = s_app_anc_gain_ctx.ed_fb_local;
    apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN, APP_EXTEND_GAIN_ENVIRONMENT_DETECTION_CHANGE,
                                   (void *)&extra, sizeof(app_anc_extend_gain_sync_t));

    app_anc_environment_detection_aws_sync_t aws_sync = {0};
    aws_sync.ed_level = s_app_anc_gain_ctx.ed_level_local;
    stationary_noise = audio_environment_detection_get_stationary_noise();
    aws_sync.stationary_noise = stationary_noise;
    apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN,
                                   APP_EXTEND_GAIN_ENVIRONMENT_DETECTION_AWS_SYNC,
                                   (void *)&aws_sync, sizeof(app_anc_environment_detection_aws_sync_t));
#endif
    APPS_LOG_MSGID_I(LOG_TAG" partner_report, wn=%d uu=%d ed=%d/%d-%d-%d", 6, s_app_anc_gain_ctx.wn_local,
                     s_app_anc_gain_ctx.uu_local, s_app_anc_gain_ctx.ed_ff_local,
                     s_app_anc_gain_ctx.ed_fb_local, s_app_anc_gain_ctx.ed_level_local, stationary_noise);
}

static bool app_anc_extend_gain_proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
            APPS_LOG_MSGID_I(LOG_TAG" [%02X] create", 1, role);
            if (extra_data == NULL || data_len != sizeof(app_anc_srv_result_t)) {
                APPS_LOG_MSGID_E(LOG_TAG" extra data error", 0);
                break;
            }

            if (self != NULL) {
                self->local_context = (void *)&s_app_anc_gain_ctx;
                if (self->local_context != NULL) {
                    memset(self->local_context, 0, sizeof(app_anc_extend_gain_context_t));
                    s_app_anc_gain_ctx.uu_local = APP_ANC_EXTEND_GAIN_UU_INIT;
                    s_app_anc_gain_ctx.uu_peer = APP_ANC_EXTEND_GAIN_UU_INIT;
                    s_app_anc_gain_ctx.uu_apply = APP_ANC_EXTEND_GAIN_UU_INIT;
                    s_app_anc_gain_ctx.ed_peer_stationary_noise = APP_ANC_EXTEND_GAIN_ED_INVAILD;
                }
                /* Register callback to wind noise lib. */
                hal_audio_service_hook_callback(AUDIO_MESSAGE_TYPE_ADAPTIVE, app_anc_extend_gain_dsp_callback, &s_app_extend_gain_monitor);
                if (role == BT_AWS_MCE_ROLE_PARTNER) {
                    app_anc_extend_gain_partner_report();
                }
                /*
                if (anc_result->cur_filter_id == AUDIO_ANC_CONTROL_FILTER_ID_ANC_4) {
                    ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN,
                                        APP_EXTEND_GAIN_USER_UNAWARE_INIT,
                                        NULL, 0, NULL, APP_ANC_EXTEND_GAIN_UU_INIT_TIMEOUT);
                }
                */
            }
#ifdef APP_ANC_EXTEND_GAIN_TEST
            atci_status_t ret = atci_register_handler(app_anc_gain_atci_cmd, sizeof(app_anc_gain_atci_cmd) / sizeof(atci_cmd_hdlr_item_t));
            APPS_LOG_MSGID_I(LOG_TAG" init atci ret=%d", 1, ret);
#endif
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(LOG_TAG" destroy", 0);
            hal_audio_service_unhook_callback(AUDIO_MESSAGE_TYPE_ADAPTIVE);
            break;
        }
        default:
            break;
    }
    return ret;
}

static bool app_anc_extend_gain_proc_bt_cm_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
            app_anc_extend_gain_context_t *local_ctx = self->local_context;
            if (remote_update == NULL) {
                break;
            }
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER
                && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                APPS_LOG_MSGID_I(LOG_TAG" AWS Connected, WN=%d UU=%d", 2,
                                 s_app_anc_gain_ctx.wn_local, s_app_anc_gain_ctx.uu_local);
                app_anc_extend_gain_partner_report();
            } else if ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                       && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                APPS_LOG_MSGID_I(LOG_TAG" AWS Disconnect, apply local WN=%d UU=%d", 2,
                                 local_ctx->wn_local, local_ctx->uu_local);
                local_ctx->ed_peer_stationary_noise = APP_ANC_EXTEND_GAIN_ED_INVAILD;
                if (local_ctx->wn_apply != local_ctx->wn_local) {
                    local_ctx->wn_apply = local_ctx->wn_local;
                    app_anc_extend_gain_set_local(AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_WIND_NOISE, local_ctx->wn_apply,
                                                  AUDIO_ANC_CONTROL_UNASSIGNED_GAIN, 0);
                }
                if (local_ctx->uu_apply != local_ctx->uu_local) {
                    local_ctx->uu_apply = local_ctx->uu_local;
                    app_anc_extend_gain_set_local(AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_USER_UNAWARE, local_ctx->uu_apply,
                                                  AUDIO_ANC_CONTROL_UNASSIGNED_GAIN, 0);
                }
                if (local_ctx->ed_ff_apply != local_ctx->ed_ff_local
                    || local_ctx->ed_fb_apply != local_ctx->ed_fb_local) {
                    local_ctx->ed_ff_apply = local_ctx->ed_ff_local;
                    local_ctx->ed_fb_apply = local_ctx->ed_fb_local;
                    app_anc_extend_gain_set_local(AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION,
                                                  local_ctx->ed_ff_apply, local_ctx->ed_fb_apply, 0);
                }
                if (local_ctx->ed_level_apply != local_ctx->ed_level_local) {
                    local_ctx->ed_level_apply = local_ctx->ed_level_local;
                    if (role == BT_AWS_MCE_ROLE_AGENT) {
                        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN, APP_EXTEND_GAIN_NOTIFY_ENVIRONMENT_DETECTION_TO_PHONE_APP);
                        ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN,
                                            APP_EXTEND_GAIN_NOTIFY_ENVIRONMENT_DETECTION_TO_PHONE_APP,
                                            NULL, 0, NULL, 500);
                    }
                }
            }
            break;
        }
        default:
            break;
    }
    return false;
}

static bool app_anc_extend_gain_proc_audio_anc_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    if (event_id == AUDIO_ANC_CONTROL_EVENT_OFF) {
        APPS_LOG_MSGID_I(LOG_TAG" ANC OFF", 0);
        app_anc_extend_gain_set_local(AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_WIND_NOISE, 0,
                                      AUDIO_ANC_CONTROL_UNASSIGNED_GAIN, 0);
        app_anc_extend_gain_set_local(AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION, 0, 0, 0);

        g_app_extend_gain_activity_started = false;
        ui_shell_finish_activity(self, self);
    } else if (event_id == AUDIO_ANC_CONTROL_EVENT_ON) {
        app_anc_extend_gain_context_t *local_ctx = self->local_context;
        app_anc_srv_result_t *anc_result = (app_anc_srv_result_t *)extra_data;
        if (anc_result->cur_filter_id != AUDIO_ANC_CONTROL_FILTER_ID_ANC_4) {
            APPS_LOG_MSGID_I(LOG_TAG" ANC ON but not Filter_4 %d, reset UU ctx", 1, anc_result->cur_filter_id);
            local_ctx->uu_local = APP_ANC_EXTEND_GAIN_UU_INIT;
            local_ctx->uu_peer = APP_ANC_EXTEND_GAIN_UU_INIT;
            local_ctx->uu_apply = APP_ANC_EXTEND_GAIN_UU_INIT;
        } else {
            APPS_LOG_MSGID_I(LOG_TAG" ANC ON Filter_4 %d, send UU init event", 1, anc_result->cur_filter_id);
            /*
            ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN,
                                APP_EXTEND_GAIN_USER_UNAWARE_INIT,
                                NULL, 0, NULL, APP_ANC_EXTEND_GAIN_UU_INIT_TIMEOUT);
            */
        }
    }
    return false;
}

static bool app_anc_extend_gain_proc_interaction_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    app_anc_extend_gain_context_t *local_ctx = self->local_context;

    switch (event_id) {
        case APPS_EVENTS_INTERACTION_RHO_END: {
            app_rho_result_t rho_result = (app_rho_result_t)extra_data;
            if (rho_result == APP_RHO_RESULT_FAIL) {
                break;
            }
            APPS_LOG_MSGID_I(LOG_TAG" RHO_END, sync gain %d to new agent", 1, local_ctx->wn_local);
            /* New partner sync gain to agent, only new partner receive this event. */
            app_anc_extend_gain_partner_report();
            break;
        }
#ifdef MTK_IN_EAR_FEATURE_ENABLE
        case APPS_EVENTS_INTERACTION_UPDATE_IN_EAR_STA_EFFECT: {
            if (extra_data) {
                bool *in_ear = (bool *)extra_data;
                APPS_LOG_MSGID_I(LOG_TAG" in ear state %d, ed_enable %d", 2, *in_ear, s_app_anc_gain_control.ed_enable);
#ifdef AIR_ANC_USER_UNAWARE_ENABLE
                if (*in_ear && s_app_anc_gain_control.ed_enable) {
#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
                    if (s_app_anc_gain_control.ed_enable) {
                        /* Set ED as 0 to ensure UU work. */
                        app_anc_extend_gain_set_local(AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION, 0, 0, 0);
                    }
#endif
                    s_app_anc_gain_ctx.uu_ed_mode = APP_UU_ED_MODE_INIT;
                    audio_user_unaware_enable(TRUE);
                    /*
                    ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN,
                                        APP_EXTEND_GAIN_USER_UNAWARE_INIT,
                                        NULL, 0, NULL, APP_ANC_EXTEND_GAIN_UU_INIT_TIMEOUT);
                    */
                }
#endif
            }
            break;
        }
#endif
        default:
            break;
    }
    return false;
}


static void app_anc_extend_gain_update_wn_gain(app_anc_extend_gain_context_t *local_ctx)
{
    audio_anc_control_gain_t apply_gain = (local_ctx->wn_local <= local_ctx->wn_peer ? local_ctx->wn_local : local_ctx->wn_peer);
    APPS_LOG_MSGID_I(LOG_TAG" update_wn_gain, local=%d peer=%d apply=%d->%d",
                     4, local_ctx->wn_local, local_ctx->wn_peer,
                     local_ctx->wn_apply, apply_gain);
    if (local_ctx->wn_apply != apply_gain) {
        app_anc_extend_gain_set_and_sync(AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_WIND_NOISE, apply_gain,
                                         AUDIO_ANC_CONTROL_UNASSIGNED_GAIN, APP_ANC_EXTEND_GAIN_SYNC_DELAY);
        local_ctx->wn_apply = apply_gain;
    }
}

static void app_anc_extend_gain_update_uu_gain(app_anc_extend_gain_context_t *local_ctx)
{
    audio_anc_control_gain_t apply_gain = (local_ctx->uu_local <= local_ctx->uu_peer ? local_ctx->uu_local : local_ctx->uu_peer);
    APPS_LOG_MSGID_I(LOG_TAG" update_uu_gain, local=%d peer=%d apply=%d->%d",
                     4, local_ctx->uu_local, local_ctx->uu_peer, local_ctx->uu_apply, apply_gain);
    if (local_ctx->uu_apply != apply_gain) {
        //if (local_ctx->uu_peer != APP_ANC_EXTEND_GAIN_UU_INIT) {
        if (1) {
            app_anc_extend_gain_set_and_sync(AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_USER_UNAWARE, apply_gain,
                                             AUDIO_ANC_CONTROL_UNASSIGNED_GAIN, APP_ANC_EXTEND_GAIN_SYNC_DELAY);
        } else {
            app_anc_extend_gain_set_local(AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_USER_UNAWARE, apply_gain,
                                          AUDIO_ANC_CONTROL_UNASSIGNED_GAIN, 0);
        }

        if (local_ctx->uu_local != APP_ANC_EXTEND_GAIN_UU_INIT && apply_gain != APP_ANC_EXTEND_GAIN_UU_INIT) {
            local_ctx->uu_apply = apply_gain;
        }
    }
}

static void app_anc_extend_gain_update_ed_gain(app_anc_extend_gain_context_t *local_ctx)
{
    audio_anc_control_gain_t ff_apply_gain = (local_ctx->ed_ff_local > local_ctx->ed_ff_peer ? local_ctx->ed_ff_local : local_ctx->ed_ff_peer);
    audio_anc_control_gain_t fb_apply_gain = (local_ctx->ed_fb_local > local_ctx->ed_fb_peer ? local_ctx->ed_fb_local : local_ctx->ed_fb_peer);
    APPS_LOG_MSGID_I(LOG_TAG" update_ed_gain, local=%d/%d peer=%d/%d apply=%d/%d->%d/%d",
                     8, local_ctx->ed_ff_local, local_ctx->ed_fb_local,
                     local_ctx->ed_ff_peer, local_ctx->ed_fb_peer,
                     local_ctx->ed_ff_apply, local_ctx->ed_fb_apply,
                     ff_apply_gain, fb_apply_gain);
    if (local_ctx->ed_ff_apply != ff_apply_gain || local_ctx->ed_fb_apply != fb_apply_gain) {
        app_anc_extend_gain_set_and_sync(AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION,
                                         ff_apply_gain, fb_apply_gain,
                                         APP_ANC_EXTEND_GAIN_SYNC_DELAY);
        local_ctx->ed_ff_apply = ff_apply_gain;
        local_ctx->ed_fb_apply = fb_apply_gain;
    }
}

static void app_anc_extend_gain_update_gain(app_anc_extend_gain_event_t event, app_anc_extend_gain_context_t *local_ctx)
{
    if (event == APP_EXTEND_GAIN_WIND_NOISE_CHANGE) {
        app_anc_extend_gain_update_wn_gain(local_ctx);
    } else if (event == APP_EXTEND_GAIN_USER_UNAWARE_CHANGE) {
        app_anc_extend_gain_update_uu_gain(local_ctx);
    } else if (event == APP_EXTEND_GAIN_ENVIRONMENT_DETECTION_CHANGE) {
        app_anc_extend_gain_update_ed_gain(local_ctx);
    }
}

static bool app_anc_extend_gain_proc_aws_data_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;

    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        uint32_t event_group = 0;
        uint32_t event_id = 0;
        app_anc_extend_gain_sync_t *extra = NULL;
        uint32_t extra_len = 0;

        apps_aws_sync_event_decode_extra(aws_data_ind, &event_group, &event_id, (void *)&extra, &extra_len);
        if (event_group == EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN && extra != NULL) {
            app_anc_extend_gain_context_t *local_ctx = (app_anc_extend_gain_context_t *)self->local_context;
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();

            if (event_id == APP_EXTEND_GAIN_ENVIRONMENT_DETECTION_AWS_SYNC) {
                app_anc_environment_detection_aws_sync_t aws_sync = *((app_anc_environment_detection_aws_sync_t *)extra);
                APPS_LOG_MSGID_I(LOG_TAG" [AWS_DATA] [%02X] ed_level sync level=%d stationary_noise=%d",
                                 3, role, aws_sync.ed_level, aws_sync.stationary_noise);
                app_anc_extend_gain_handle_ed_level(FALSE, aws_sync.ed_level);
                s_app_anc_gain_ctx.ed_peer_stationary_noise = aws_sync.stationary_noise;
                return TRUE;
            } else if (event_id == APP_EXTEND_GAIN_ED_NOTIFY_ED_NOISE) {
                app_anc_environment_detection_noise_sync_t noise_sync = *((app_anc_environment_detection_noise_sync_t *)extra);
                int16_t stationary_noise = noise_sync.stationary_noise;
                bool is_response = noise_sync.is_response;
                APPS_LOG_MSGID_I(LOG_TAG" [AWS_DATA] [%02X] ed_nosie stationary_noise=%d is_response=%d",
                                 3, role, stationary_noise, is_response);
                s_app_anc_gain_ctx.ed_peer_stationary_noise = stationary_noise;

                if (role == BT_AWS_MCE_ROLE_PARTNER) {
                    app_anc_extend_gain_notify_ed_noise(is_response);
                } else if (role == BT_AWS_MCE_ROLE_AGENT) {
                    app_anc_extend_gain_notify_ed_result_imp(is_response);
                }
                return TRUE;
            }

            audio_anc_control_gain_t gain_1 = (audio_anc_control_gain_t)extra->gain_1;
            audio_anc_control_gain_t gain_2 = (audio_anc_control_gain_t)extra->gain_2;
            APPS_LOG_MSGID_I(LOG_TAG" [AWS_DATA] [%02X] event=%d gain_1=%d gain_2=%d",
                             4, role, event_id, gain_1, gain_2);

            if (role == BT_AWS_MCE_ROLE_AGENT) {
                if (event_id == APP_EXTEND_GAIN_WIND_NOISE_CHANGE) {
                    local_ctx->wn_peer = gain_1;
                } else if (event_id == APP_EXTEND_GAIN_USER_UNAWARE_CHANGE) {
                    if (local_ctx->uu_ed_mode == APP_UU_ED_MODE_ED) {
                        return TRUE;
                    }
                    local_ctx->uu_peer = gain_1;
                } else if (event_id == APP_EXTEND_GAIN_ENVIRONMENT_DETECTION_CHANGE) {
                    local_ctx->ed_ff_peer = gain_1;
                    local_ctx->ed_fb_peer = gain_2;
                }
                /* Agent received partner's local gain, need compare with local gain and decide. */
                app_anc_extend_gain_update_gain(event_id, local_ctx);
            } else if (role == BT_AWS_MCE_ROLE_PARTNER) {
                /* Partner receive agent's decision gain. */
                uint32_t target_gpt = 0;
                audio_anc_control_extend_ramp_gain_type_t gain_type = 0;
                if (event_id == APP_EXTEND_GAIN_WIND_NOISE_CHANGE) {
                    gain_type = AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_WIND_NOISE;
                    local_ctx->wn_apply = gain_1;
                    gain_2 = AUDIO_ANC_CONTROL_UNASSIGNED_GAIN;
                } else if (event_id == APP_EXTEND_GAIN_USER_UNAWARE_CHANGE) {
                    if (local_ctx->uu_ed_mode == APP_UU_ED_MODE_ED) {
                        return TRUE;
                    }
                    gain_type = AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_USER_UNAWARE;
                    local_ctx->uu_apply = gain_1;
                    gain_2 = AUDIO_ANC_CONTROL_UNASSIGNED_GAIN;
                } else if (event_id == APP_EXTEND_GAIN_ENVIRONMENT_DETECTION_CHANGE) {
                    gain_type = AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION;
                    local_ctx->ed_ff_apply = gain_1;
                    local_ctx->ed_fb_apply = gain_2;
                }
                APPS_LOG_MSGID_I(LOG_TAG" [AWS_DATA] bt_clk 0x%08x 0x%08x", 2, extra->bt_clk.nclk, extra->bt_clk.nclk_intra);
                if (bt_sink_srv_convert_bt_clock_2_gpt_count((const bt_clock_t *)&extra->bt_clk, &target_gpt) == BT_STATUS_SUCCESS) {
                    app_anc_extend_gain_set_local(gain_type, gain_1, gain_2, target_gpt);
                } else {
                    APPS_LOG_MSGID_E(LOG_TAG" [AWS_DATA] bt_clk to gpt convert fail, set 0", 0);
                    app_anc_extend_gain_set_local(gain_type, gain_1, gain_2, 0);
                }
            }

            ret = TRUE;
        } else if (event_group == EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN_RACE && extra != NULL) {
            if (event_id == RACE_EVENT_TYPE_ANC_GAIN_CONTROL) {
                anc_gain_control_param_t param = *((anc_gain_control_param_t *)extra);
                app_anc_extend_gain_control(&param);
            }
            ret = TRUE;
        }
    }
    return ret;
}

static bool app_anc_extend_gain_proc_anc_extend_gain_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    app_anc_extend_gain_context_t *local_ctx = (app_anc_extend_gain_context_t *)self->local_context;
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    switch (event_id) {
        case APP_EXTEND_GAIN_WIND_NOISE_CHANGE:
        case APP_EXTEND_GAIN_USER_UNAWARE_CHANGE:
        case APP_EXTEND_GAIN_ENVIRONMENT_DETECTION_CHANGE: {
            uint32_t gain = (uint32_t)extra_data;
            audio_anc_control_gain_t gain_1 = (audio_anc_control_gain_t)(gain & 0xFFFF);
            audio_anc_control_gain_t gain_2 = 0;
            if (event_id == APP_EXTEND_GAIN_WIND_NOISE_CHANGE) {
                local_ctx->wn_local = gain_1;
            } else if (event_id == APP_EXTEND_GAIN_USER_UNAWARE_CHANGE) {
                if (local_ctx->uu_ed_mode == APP_UU_ED_MODE_ED) {
                    break;
                }

                if (role == BT_AWS_MCE_ROLE_PARTNER && local_ctx->uu_local == APP_ANC_EXTEND_GAIN_UU_INIT &&
                   !(local_ctx->uu_apply == 1 && gain_1 == 0)) {
                    /* BTA-20707, Agent will not update uu_apply, so Partner should apply uu_gain by itself. */
                    local_ctx->uu_local = gain_1;
                    app_anc_extend_gain_set_local(AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_USER_UNAWARE,
                                                  local_ctx->uu_apply,
                                                  AUDIO_ANC_CONTROL_UNASSIGNED_GAIN, 0);
                } else {
                    local_ctx->uu_local = gain_1;
                }
            } else if (event_id == APP_EXTEND_GAIN_ENVIRONMENT_DETECTION_CHANGE) {
                app_anc_environment_detection_param_t *environment_detection_param = (app_anc_environment_detection_param_t *)extra_data;
                gain_1 = environment_detection_param->ff_gain;
                gain_2 = environment_detection_param->fb_gain;
                local_ctx->ed_ff_local = gain_1;
                local_ctx->ed_fb_local = gain_2;
                app_anc_extend_gain_handle_ed_level(TRUE, environment_detection_param->ed_level);
            }

            if (BT_AWS_MCE_SRV_LINK_NONE != bt_aws_mce_srv_get_link_type()) {
                APPS_LOG_MSGID_I(LOG_TAG" [ANC_GAIN event] Local Change, [%02X] AWS_Connected event=%d gain_1=%d gain_2=%d",
                                 4, role, event_id, gain_1, gain_2);
                if (role == BT_AWS_MCE_ROLE_AGENT) {
                    app_anc_extend_gain_update_gain(event_id, local_ctx);
                } else if (role == BT_AWS_MCE_ROLE_PARTNER) {
                    app_anc_extend_gain_sync_t extra = {0};
                    extra.gain_1 = gain_1;
                    extra.gain_2 = gain_2;
                    apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN, event_id,
                                                   (void *)&extra, sizeof(app_anc_extend_gain_sync_t));
                }
            } else {
                APPS_LOG_MSGID_I(LOG_TAG" [ANC_GAIN event] Local Change, [%02X] AWS_Disconnected event=%d gain_1=%d gain_2=%d",
                                 4, role, event_id, gain_1, gain_2);
                if (event_id == APP_EXTEND_GAIN_WIND_NOISE_CHANGE
                        && local_ctx->wn_local != local_ctx->wn_apply) {
                    local_ctx->wn_apply = local_ctx->wn_local;
                    app_anc_extend_gain_set_local(AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_WIND_NOISE,
                                                  local_ctx->wn_apply,
                                                  AUDIO_ANC_CONTROL_UNASSIGNED_GAIN, 0);
                } else if (event_id == APP_EXTEND_GAIN_USER_UNAWARE_CHANGE
                        && local_ctx->uu_local != local_ctx->uu_apply) {
                    local_ctx->uu_apply = local_ctx->uu_local;
                    app_anc_extend_gain_set_local(AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_USER_UNAWARE,
                                                  local_ctx->uu_apply,
                                                  AUDIO_ANC_CONTROL_UNASSIGNED_GAIN, 0);
                } else if (event_id == APP_EXTEND_GAIN_ENVIRONMENT_DETECTION_CHANGE
                        && (local_ctx->ed_ff_local != local_ctx->ed_ff_apply
                                || local_ctx->ed_fb_local != local_ctx->ed_fb_apply)) {
                    local_ctx->ed_ff_apply = local_ctx->ed_ff_local;
                    local_ctx->ed_fb_apply = local_ctx->ed_fb_local;
                    app_anc_extend_gain_set_local(AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION,
                                                  local_ctx->ed_ff_apply, local_ctx->ed_fb_apply, 0);
                }
            }
            break;
        }

        case APP_EXTEND_GAIN_USER_UNAWARE_SET: {
            APPS_LOG_MSGID_I(LOG_TAG" [ANC_GAIN event] USER_UNAWARE_SET, [%02X] gain=%d ed_enable=%d",
                             3, role, extra_data, s_app_anc_gain_control.ed_enable);
            audio_anc_monitor_set_info(AUDIO_ANC_MONITOR_SET_USER_UNAWARE_STAT, (uint32_t)extra_data);
#if (defined AIR_ANC_USER_UNAWARE_ENABLE) && (defined AIR_ANC_ENVIRONMENT_DETECTION_ENABLE)
            /* Disable UU and switch to environment detection when it's tight. */
            if (extra_data == 0) {
                if (s_app_anc_gain_control.ed_enable) {
                    audio_user_unaware_enable(FALSE);
                    local_ctx->uu_ed_mode = APP_UU_ED_MODE_ED;

                    local_ctx->uu_local = APP_ANC_EXTEND_GAIN_UU_INIT;
                    local_ctx->uu_peer = APP_ANC_EXTEND_GAIN_UU_INIT;
                    local_ctx->uu_apply = APP_ANC_EXTEND_GAIN_UU_INIT;
                    app_anc_extend_gain_set_local(AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION,
                                                  local_ctx->ed_ff_apply, local_ctx->ed_fb_apply, 0);
                }

                bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
                if (role == BT_AWS_MCE_ROLE_AGENT) {
                    APPS_LOG_MSGID_I(LOG_TAG" [40] notify_ed_result, when UU loose->tight", 0);
                    app_anc_extend_gain_notify_ed_result(FALSE);
                }
            } else {
                local_ctx->uu_ed_mode = APP_UU_ED_MODE_UU;
            }
#endif
            break;
        }
/*
        case APP_EXTEND_GAIN_USER_UNAWARE_INIT: {
            APPS_LOG_MSGID_I(LOG_TAG" [ANC_GAIN event] USER_UNAWARE_INIT, init to tight(0) [%02X]", 1, role);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN,
                                APP_EXTEND_GAIN_USER_UNAWARE_CHANGE,
                                (void *)0, 0, NULL, 0);
            break;
        }
*/
        case APP_EXTEND_GAIN_NOTIFY_WIND_NOISE_TO_PHONE_APP: {
            APPS_LOG_MSGID_I(LOG_TAG" [ANC_GAIN event] NOTIFY_WIND_NOISE_TO_PHONE_APP, [%02X]", 1, role);
            app_anc_extend_gain_notify_wn_result(FALSE);
            break;
        }
        case APP_EXTEND_GAIN_NOTIFY_USER_UNAWARE_TO_PHONE_APP: {
            APPS_LOG_MSGID_I(LOG_TAG" [ANC_GAIN event] NOTIFY_USER_UNAWARE_TO_PHONE_APP, [%02X]", 1, role);
            app_anc_extend_gain_notify_uu_result(FALSE);
            break;
        }
        case APP_EXTEND_GAIN_NOTIFY_ENVIRONMENT_DETECTION_TO_PHONE_APP: {
            APPS_LOG_MSGID_I(LOG_TAG" [ANC_GAIN event] NOTIFY_ENVIRONMENT_DETECTION_TO_PHONE_APP, [%02X]", 1, role);
            app_anc_extend_gain_notify_ed_result(FALSE);
            break;
        }

        default:
            break;
    }
    return true;
}

static bool app_anc_extend_gain_proc_anc_extend_gain_race_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    APPS_LOG_MSGID_I(LOG_TAG" [ANC_GAIN_RACE event] [%02X] event_id=%d", 2, role, event_id);
    switch (event_id) {
        case RACE_EVENT_TYPE_ANC_GAIN_USER_UNAWARE: {
            app_anc_extend_gain_notify_uu_result(TRUE);
            break;
        }
        case RACE_EVENT_TYPE_ANC_GAIN_WIND_NOISE: {
            app_anc_extend_gain_notify_wn_result(TRUE);
            break;
        }
        case RACE_EVENT_TYPE_ANC_GAIN_ENVIRONMENT_DETECTION: {
            app_anc_extend_gain_notify_ed_result(TRUE);
            break;
        }
        case RACE_EVENT_TYPE_ANC_GAIN_CONTROL: {
            anc_gain_control_param_t *param = (anc_gain_control_param_t *)extra_data;
            app_anc_extend_gain_control(param);
            break;
        }
    }
    return true;
}

bool app_anc_extend_gain_activity_proc(struct _ui_shell_activity *self,
                                       uint32_t event_group,
                                       uint32_t event_id,
                                       void *extra_data,
                                       size_t data_len)
{
    bool ret = false;
    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM: {
            ret = app_anc_extend_gain_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            ret = app_anc_extend_gain_proc_interaction_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER: {
            ret = app_anc_extend_gain_proc_bt_cm_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_AWS_DATA: {
            ret = app_anc_extend_gain_proc_aws_data_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_AUDIO_ANC: {
            ret = app_anc_extend_gain_proc_audio_anc_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN:
            ret = app_anc_extend_gain_proc_anc_extend_gain_group(self, event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN_RACE:
            ret = app_anc_extend_gain_proc_anc_extend_gain_race_group(self, event_id, extra_data, data_len);
            break;
        default:
            break;
    }
    return ret;
}

#ifdef APP_ANC_EXTEND_GAIN_TEST

static atci_status_t app_anc_gain_atci_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    switch (parse_cmd->mode)
    {
        case ATCI_CMD_MODE_EXECUTION: {
            char *atcmd = parse_cmd->string_ptr + parse_cmd->name_len + 1;
            char cmd[20] = {0};
            uint8_t copy_len = strlen(atcmd) - 2;
            memcpy(cmd, atcmd, (copy_len > 19 ? 19 : copy_len));
            APPS_LOG_I(LOG_TAG" AT CMD=%s", cmd);
            if (strstr(cmd, "send") > 0) {
                int type = 0;
                int gain1 = 0;
                int gain2 = 0;
                int ed_level = 0;
                sscanf(cmd, "send,%d,%d,%d,%d", &type, &gain1, &gain2, &ed_level);
                app_anc_extend_gain_handle_callback(type, gain1, gain2, ed_level);
            } else {
                APPS_LOG_MSGID_I(LOG_TAG" invalid ANC_GAIN AT-CMD", 0);
            }
            memset(response.response_buf, 0, ATCI_UART_TX_FIFO_BUFFER_SIZE);
            snprintf((char *)response.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "OK - %s\r\n", atcmd);
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            break;
        }
        default:
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            break;
    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

#endif
