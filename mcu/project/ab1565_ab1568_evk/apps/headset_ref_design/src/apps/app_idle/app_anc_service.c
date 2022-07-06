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
#ifdef MTK_ANC_ENABLE

#include "app_anc_service.h"
#include "apps_debug.h"
#include "ui_shell_manager.h"
#include "apps_events_event_group.h"
#include "apps_config_vp_index_list.h"
#include "apps_config_vp_manager.h"
#include "FreeRTOS.h"
#include "portable.h"

#ifdef MTK_AWS_MCE_ENABLE
#include "bt_device_manager.h"
#endif

#define LOG_TAG     "[ANC_SRV]"

typedef enum {
    APP_ANC_STATE_NONE       = 0,
    APP_ANC_STATE_DISABLED,
    APP_ANC_STATE_ENABLED,
} app_anc_state_t;

typedef struct {
    app_anc_state_t                     anc_state;
    uint8_t                             cur_enable;
    audio_anc_control_filter_id_t       cur_filter_id;
    audio_anc_control_type_t            cur_type;
    int16_t                             cur_runtime_gain;
    bool                                user_trigger_testing;
} app_anc_service_context_t;

static app_anc_service_context_t        g_anc_srv_context;

static void app_anc_srv_update_parameter()
{
    uint8_t                         anc_enable = 0;
    audio_anc_control_filter_id_t   anc_filter_id = AUDIO_ANC_CONTROL_FILTER_ID_NONE;
    audio_anc_control_type_t        anc_type = AUDIO_ANC_CONTROL_TYPE_DUMMY;
    int16_t                         anc_runtime_gain = 0;
    uint8_t                         support_hybrid_enable = 0;
    audio_anc_control_misc_t        anc_control_misc = {0};

    audio_anc_control_get_status(&anc_enable,
                                 &anc_filter_id,
                                 &anc_type,
                                 &anc_runtime_gain,
                                 &support_hybrid_enable,
                                 &anc_control_misc);
    APPS_LOG_MSGID_I(LOG_TAG" update_parameter, enable=%d filter_id=%d type=%d runtime_gain=%d support_hybrid=%d misc=0x%08X",
            6, anc_enable, anc_filter_id, anc_type, anc_runtime_gain, support_hybrid_enable, anc_control_misc.extend_use_parameters);
    g_anc_srv_context.cur_enable = anc_enable;
    g_anc_srv_context.cur_filter_id = anc_filter_id;
    g_anc_srv_context.cur_type = anc_type;
    g_anc_srv_context.cur_runtime_gain = anc_runtime_gain;
    if (anc_type == AUDIO_ANC_CONTROL_TYPE_HYBRID && support_hybrid_enable == 0) {
        g_anc_srv_context.cur_type = AUDIO_ANC_CONTROL_TYPE_FF;
    }
}

static void app_anc_srv_control_callback(audio_anc_control_event_t event_id, audio_anc_control_result_t result)
{
    APPS_LOG_MSGID_I(LOG_TAG" control_callback event_id=%d result=%d", 2, event_id, result);
    if (result == AUDIO_ANC_CONTROL_EXECUTION_SUCCESS) {
        if (event_id == AUDIO_ANC_CONTROL_EVENT_ON) {
            g_anc_srv_context.anc_state = APP_ANC_STATE_ENABLED;
            app_anc_srv_update_parameter();
        } else if (event_id == AUDIO_ANC_CONTROL_EVENT_OFF) {
            g_anc_srv_context.anc_state = APP_ANC_STATE_DISABLED;
            g_anc_srv_context.cur_enable = FALSE;
        }

        app_anc_srv_result_t *anc_result = (app_anc_srv_result_t *)pvPortMalloc(sizeof(app_anc_srv_result_t));
        if (anc_result != NULL) {
            anc_result->cur_filter_id = g_anc_srv_context.cur_filter_id;
            anc_result->cur_type = g_anc_srv_context.cur_type;
            anc_result->cur_runtime_gain = g_anc_srv_context.cur_runtime_gain;

            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST,
                                EVENT_GROUP_UI_SHELL_AUDIO_ANC,
                                event_id, anc_result, sizeof(app_anc_srv_result_t),
                                NULL, 0);
        }
    }
}

void app_anc_service_init(void)
{
    audio_anc_control_result_t anc_result = audio_anc_control_register_callback(
                                          app_anc_srv_control_callback,
                                          AUDIO_ANC_CONTROL_EVENT_ON
                                          | AUDIO_ANC_CONTROL_EVENT_OFF
                                          | AUDIO_ANC_CONTROL_EVENT_COPY_FILTER
                                          | AUDIO_ANC_CONTROL_EVENT_SET_REG
                                          | AUDIO_ANC_CONTROL_EVENT_FORCE_OFF
                                          | AUDIO_ANC_CONTROL_EVENT_HOWLING,
                                          AUDIO_ANC_CONTROL_CB_LEVEL_ALL);
    APPS_LOG_MSGID_I(LOG_TAG" init, register_callback anc_result=%d", 1, anc_result);

    anc_result = audio_anc_control_get_status_from_flash(&g_anc_srv_context.cur_enable,
                    &g_anc_srv_context.cur_filter_id,
                    &g_anc_srv_context.cur_type,
                    &g_anc_srv_context.cur_runtime_gain,
                    NULL);
    if (anc_result != AUDIO_ANC_CONTROL_EXECUTION_SUCCESS) {
        APPS_LOG_MSGID_E(LOG_TAG" init, get_status_from_flash fail anc_result=%d", 1, anc_result);
    }
    APPS_LOG_MSGID_I(LOG_TAG" init, enable=%d filter_id=%d type=%d runtime_gain=%d",
            4, g_anc_srv_context.cur_enable, g_anc_srv_context.cur_filter_id,
            g_anc_srv_context.cur_type, g_anc_srv_context.cur_runtime_gain);

    g_anc_srv_context.anc_state = APP_ANC_STATE_NONE;
    g_anc_srv_context.user_trigger_testing = false;

    if (g_anc_srv_context.cur_enable == 0) {
        APPS_LOG_MSGID_I(LOG_TAG" init, no need to disable", 0);
        return;
    }

#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    if (role == BT_AWS_MCE_ROLE_AGENT) {
        anc_result += audio_anc_control_set_runtime_gain(g_anc_srv_context.cur_runtime_gain, g_anc_srv_context.cur_type);
        anc_result += audio_anc_control_enable(g_anc_srv_context.cur_filter_id, g_anc_srv_context.cur_type, NULL);
    }
    APPS_LOG_MSGID_I(LOG_TAG" init, role=0x%02X anc_result=%d", 2, role, anc_result);
#else
    anc_result += audio_anc_control_set_runtime_gain(g_anc_srv_context.cur_runtime_gain, g_anc_srv_context.cur_type);
    anc_result += audio_anc_control_enable(g_anc_srv_context.cur_filter_id, g_anc_srv_context.cur_type, NULL);
    APPS_LOG_MSGID_I(LOG_TAG" init, anc_result=%d", 1, anc_result);
#endif
}

void app_anc_service_save_into_flash(void)
{
    app_anc_srv_update_parameter();

    audio_anc_control_result_t anc_result = audio_anc_control_set_status_into_flash(
                                                g_anc_srv_context.cur_enable,
                                                g_anc_srv_context.cur_filter_id,
                                                g_anc_srv_context.cur_type,
                                                g_anc_srv_context.cur_runtime_gain,
                                                NULL);
    APPS_LOG_MSGID_I(LOG_TAG" save_into_flash, anc_result=%d", 1, anc_result);
}

void app_anc_service_play_vp(void)
{
    APPS_LOG_MSGID_I(LOG_TAG" play VP", 0);
    apps_config_set_vp(VP_INDEX_SUCCESSED, true, 200, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
}

bool app_anc_service_is_enable(void)
{
    return (g_anc_srv_context.cur_enable > 0);
}

bool app_anc_service_enable(audio_anc_control_filter_id_t filter_id,
        audio_anc_control_type_t anc_type,
        audio_anc_control_gain_t runtime_gain,
        audio_anc_control_misc_t *control_misc)
{
    if (g_anc_srv_context.user_trigger_testing) {
        APPS_LOG_MSGID_W(LOG_TAG" user trigger test ongoing, cannot enable ANC", 0);
        return false;
    }

    audio_anc_control_result_t anc_result = audio_anc_control_set_runtime_gain(runtime_gain, anc_type);
    anc_result += audio_anc_control_enable(filter_id, anc_type, control_misc);
    APPS_LOG_MSGID_I(LOG_TAG" enable, filter_id=%d type=%d runtime_gain=%d(0x%08X) anc_result=%d",
                     5, filter_id, anc_type, runtime_gain, runtime_gain, anc_result);
    app_anc_service_play_vp();
    return (anc_result == AUDIO_ANC_CONTROL_EXECUTION_SUCCESS);
}

bool app_anc_service_disable()
{
    if (g_anc_srv_context.user_trigger_testing) {
        APPS_LOG_MSGID_W(LOG_TAG" user trigger test ongoing, cannot control ANC", 0);
        return false;
    }

    audio_anc_control_result_t anc_result = audio_anc_control_disable(NULL);
    APPS_LOG_MSGID_I(LOG_TAG" disable, anc_result=%d", 1, anc_result);
    app_anc_service_play_vp();
    return (anc_result == AUDIO_ANC_CONTROL_EXECUTION_SUCCESS);
}

bool app_anc_service_set_runtime_gain(audio_anc_control_type_t anc_type,
        audio_anc_control_gain_t runtime_gain)
{
    if (g_anc_srv_context.user_trigger_testing) {
        APPS_LOG_MSGID_W(LOG_TAG" user trigger test ongoing, cannot set runtime gain", 0);
        return false;
    }

    audio_anc_control_result_t anc_result = audio_anc_control_set_runtime_gain(runtime_gain, anc_type);
    if (anc_result == AUDIO_ANC_CONTROL_EXECUTION_SUCCESS) {
        g_anc_srv_context.cur_runtime_gain = runtime_gain;
    }
    APPS_LOG_MSGID_I(LOG_TAG" set_runtime_gain anc_result=%d", 1, anc_result);
    return (anc_result == AUDIO_ANC_CONTROL_EXECUTION_SUCCESS);
}

bool app_anc_service_suspend(void)
{
    if (g_anc_srv_context.user_trigger_testing) {
        APPS_LOG_MSGID_W(LOG_TAG" user trigger test ongoing, cannot suspend ANC", 0);
        return false;
    }

    app_anc_srv_update_parameter();

    audio_anc_control_misc_t anc_enable_param;
    anc_enable_param.disable_with_suspend = true;
    audio_anc_control_result_t anc_result = audio_anc_control_disable(&anc_enable_param);
    APPS_LOG_MSGID_I(LOG_TAG" suspend, anc_result=%d", 1, anc_result);
    return (anc_result == AUDIO_ANC_CONTROL_EXECUTION_SUCCESS);
}

bool app_anc_service_resume(void)
{
    if (g_anc_srv_context.user_trigger_testing) {
        APPS_LOG_MSGID_W(LOG_TAG" user trigger test ongoing, cannot resume ANC", 0);
        return false;
    }

    app_anc_srv_update_parameter();

    audio_anc_control_misc_t anc_enable_param;
    anc_enable_param.enable_with_resume = true;
    audio_anc_control_result_t anc_result = audio_anc_control_enable(g_anc_srv_context.cur_filter_id, g_anc_srv_context.cur_type, &anc_enable_param);
    APPS_LOG_MSGID_I(LOG_TAG" resume, anc_result=%d", 1, anc_result);
    return (anc_result == AUDIO_ANC_CONTROL_EXECUTION_SUCCESS);
}

bool app_anc_service_reinit_nvdm()
{
    audio_anc_control_result_t anc_result = audio_anc_control_reinit_nvdm();
    APPS_LOG_MSGID_I(LOG_TAG" reinit_nvdm result=%d", 1, anc_result);
    return (anc_result == AUDIO_ANC_CONTROL_EXECUTION_SUCCESS);
}

void app_anc_service_set_user_trigger_state(bool ongoing)
{
    APPS_LOG_MSGID_I(LOG_TAG" set user trigger state=%d", 1, ongoing);
    g_anc_srv_context.user_trigger_testing = ongoing;
}

app_anc_service_state_t app_anc_server_get_state(void)
{
    app_anc_service_state_t state = APP_ANC_STATE_DISABLE;
    uint8_t                         anc_enable = 0;
    audio_anc_control_type_t        anc_type = AUDIO_ANC_CONTROL_TYPE_DUMMY;
    audio_anc_control_get_status(&anc_enable, NULL, &anc_type,
                                 NULL, NULL, NULL);

    if (anc_enable && anc_type < AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF) {
        state = APP_ANC_STATE_ANC_ENABLE;
    } else if (anc_enable && anc_type == AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF) {
        state = APP_ANC_STATE_PT_ENABLE;
    }
    APPS_LOG_MSGID_I(LOG_TAG" [ADVANCE_PT] get_state, anc_enable=%d anc_type=%d state=%d",
                     3, anc_enable, anc_type, state);
    return state;
}

#endif /* MTK_ANC_ENABLE */
