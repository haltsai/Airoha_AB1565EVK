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

/**
 * File: apps_config_vp_manager.c
 *
 * Description: This file provides interfaces for APPs to set VP and voice.
 *
 */

#include "apps_config_vp_manager.h"
#include "apps_config_vp_index_list.h"
#include "app_voice_prompt.h"
#include "apps_debug.h"
#include "bt_device_manager.h"
#include "bt_connection_manager.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "hal_gpt.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "apps_aws_sync_event.h"
#endif

#define LOG_TAG     "[vp_manager] "

uint8_t g_app_config_set_vp = 0;

uint16_t apps_config_set_vp_by_gpt(uint32_t vp_index, uint32_t gpt_count,
        app_vp_prio_t level, bool cleanup, app_vp_play_callback_t callback)
{
    APPS_LOG_MSGID_I(LOG_TAG"Voice prompt index = 0x%x, gpt = 0x%x", 2, vp_index, gpt_count);
#ifdef MTK_PROMPT_SOUND_ENABLE
    app_vp_type cur_type = app_voice_prompt_get_current_type();
    if ((cur_type == VP_TYPE_VB || cur_type == VP_TYPE_RT) && !app_voice_prompt_get_voice_stopping() && !cleanup) {
        APPS_LOG_MSGID_I(LOG_TAG"voice playing, skip vp", 0);
        if (callback) {
            callback(vp_index, VP_ERR_CODE_FAIL);
        }
        return 0;
    }

    if (vp_index == VP_INDEX_INVALID) {
        APPS_LOG_MSGID_I(LOG_TAG"index invalid, skip vp", 0);
        if (callback) {
            callback(vp_index, VP_ERR_CODE_FAIL);
        }
        return 0;
    }

    uint32_t cur_gpt = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &cur_gpt);
    if (cur_gpt >= gpt_count) {
        gpt_count = 0;
    }
    return app_voice_prompt_play(vp_index, false, 0, level, cleanup, callback, gpt_count);
#else
    return 0;
#endif
}

uint16_t apps_config_set_vp_preempt(uint32_t vp_index, bool need_sync, uint32_t sync_delay_time,
        app_vp_prio_t level, app_vp_play_callback_t callback)
{
    APPS_LOG_MSGID_I(LOG_TAG"set VP preempt index=%d sync=%d delay=%d level=%d",
                        5, vp_index, need_sync, sync_delay_time, level);
#ifdef MTK_PROMPT_SOUND_ENABLE
    app_vp_type cur_type = app_voice_prompt_get_current_type();

    if (vp_index == VP_INDEX_INVALID) {
        APPS_LOG_MSGID_I(LOG_TAG"index invalid, skip vp", 0);
        if (callback) {
            callback(vp_index, VP_ERR_CODE_FAIL);
        }
        return 0;
    }

    if ((cur_type == VP_TYPE_VB || cur_type == VP_TYPE_RT) && !app_voice_prompt_get_voice_stopping()) {
        APPS_LOG_MSGID_I(LOG_TAG"voice playing, skip vp", 0);
        if (callback) {
            callback(vp_index, VP_ERR_CODE_FAIL);
        }
        return 0;
    } else {
        app_voice_prompt_set_preempt();
        return apps_config_set_vp(vp_index, need_sync, sync_delay_time, level, false, callback);
    }
#else
    return 0;
#endif
}

#ifdef MTK_AWS_MCE_ENABLE
bool apps_config_set_vp_to_agent(uint32_t vp_index, bool need_sync, uint32_t sync_delay_time,
                                 app_vp_prio_t level, bool is_ringtone, bool repeat)
{
    if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_PARTNER) {
        APPS_LOG_MSGID_E(LOG_TAG" Only Partner can set_vp_to_agent", 0);
        return false;
    }
    APPS_LOG_MSGID_I(LOG_TAG" set_vp_to_agent, index=%d sync=%d delay=%d level=%d, is_rt=%d, repeat=%d", 6,
            vp_index, need_sync, sync_delay_time, level, is_ringtone, repeat);

    apps_config_vp_t vp_extra = {0};
    if (!is_ringtone) {
        vp_extra.type = VP_TYPE_VP;
    } else {
        vp_extra.type = VP_TYPE_RT;
    }
    vp_extra.index = vp_index;
    vp_extra.need_sync = need_sync;
    vp_extra.level = level;
    vp_extra.delay_time = sync_delay_time;
    vp_extra.need_repeat = repeat;

    if (apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_PLAY_VP,
            (void *)&vp_extra, sizeof(apps_config_vp_t)) == BT_STATUS_SUCCESS) {
        return true;
    } else {
        return false;
    }
}
#endif

uint16_t apps_config_set_vp(uint32_t vp_index, bool need_sync, uint32_t sync_delay_time,
                            app_vp_prio_t level, bool cleanup, app_vp_play_callback_t callback)
{
    g_app_config_set_vp = 1;
    APPS_LOG_MSGID_I(LOG_TAG"set VP index=%d sync=%d delay=%d level=%d cleanup=%d",
                        5, vp_index, need_sync, sync_delay_time, level, cleanup);
#ifdef MTK_PROMPT_SOUND_ENABLE
    app_vp_type cur_type = app_voice_prompt_get_current_type();
    if ((cur_type == VP_TYPE_VB || cur_type == VP_TYPE_RT) && !app_voice_prompt_get_voice_stopping() && !cleanup) {
        APPS_LOG_MSGID_I(LOG_TAG"voice playing, skip vp", 0);
        if (callback) {
            callback(vp_index, VP_ERR_CODE_FAIL);
        }
        return 0;
    }

    if (vp_index == VP_INDEX_INVALID) {
        APPS_LOG_MSGID_I(LOG_TAG"index invalid, skip vp", 0);
        if (callback) {
            callback(vp_index, VP_ERR_CODE_FAIL);
        }
        return 0;
    }

#ifdef MTK_AWS_MCE_ENABLE
    uint32_t role = bt_device_manager_aws_local_info_get_role();
    bt_cm_power_state_t bt_power_state = bt_cm_power_get_state();

    if (role == BT_AWS_MCE_ROLE_NONE || (!need_sync) || (bt_power_state != BT_CM_POWER_STATE_ON)) {
        /* Trigger local VP. */
        return app_voice_prompt_play(vp_index, false, sync_delay_time, level, cleanup, callback, 0);
    } else {
        if (role == BT_AWS_MCE_ROLE_AGENT) {
            /* Only agent can trigger sync VP. */
            return app_voice_prompt_play(vp_index, true, sync_delay_time, level, cleanup, callback, 0);
        } else {
            /* Ignore on partner/client. */
            return 0;
        }
    }
#else
    return app_voice_prompt_play(vp_index, false, sync_delay_time, level, cleanup, callback, 0);
#endif
#else
    return 0;
#endif
}

bool apps_config_stop_vp(uint16_t id, bool need_sync, uint32_t sync_delay_time)
{
#ifdef MTK_PROMPT_SOUND_ENABLE
#ifdef MTK_AWS_MCE_ENABLE
    uint32_t role = bt_device_manager_aws_local_info_get_role();
    bt_cm_power_state_t bt_power_state = bt_cm_power_get_state();

    if (role == BT_AWS_MCE_ROLE_NONE || (!need_sync) || (bt_power_state != BT_CM_POWER_STATE_ON)) {
        /* Local stop VP. */
        return app_voice_prompt_stop(id, false, 0);
    } else {
        if (role == BT_AWS_MCE_ROLE_AGENT) {
            /* Only agent can sync stop. */
            return app_voice_prompt_stop(id, true, sync_delay_time);
        } else {
            /* Ignore on partner/client. */
            return 0;
        }
    }
#else
    return app_voice_prompt_stop(id, false, 0);
#endif
#else
    return true;
#endif
}

uint16_t apps_config_set_voice(uint32_t vp_index, bool need_sync, uint32_t sync_delay_time,
                               app_vp_prio_t level, bool ringtone, bool repeat, app_vp_play_callback_t callback)
{
#ifdef MTK_PROMPT_SOUND_ENABLE

    if (vp_index == VP_INDEX_INVALID) {
        APPS_LOG_MSGID_I(LOG_TAG"index invalid, skip vp", 0);
        if (callback) {
            callback(vp_index, VP_ERR_CODE_FAIL);
        }
        return 0;
    }

    app_vp_type cur_type = app_voice_prompt_get_current_type();
    if ((cur_type == VP_TYPE_VB || cur_type == VP_TYPE_RT) && (vp_index != app_voice_prompt_get_current_index())) {
        APPS_LOG_MSGID_I(LOG_TAG"voice playing, skip voice", 0);
        if (callback) {
            callback(vp_index, VP_ERR_CODE_FAIL);
        }
        return 0;
    }

    APPS_LOG_MSGID_I(LOG_TAG"set voice, index %d, repeat %d", 2, vp_index, repeat);

#ifdef MTK_AWS_MCE_ENABLE
    uint32_t role = bt_device_manager_aws_local_info_get_role();
    bt_cm_power_state_t bt_power_state = bt_cm_power_get_state();

    if (role == BT_AWS_MCE_ROLE_NONE || (!need_sync) || (bt_power_state != BT_CM_POWER_STATE_ON)) {
        return app_voice_prompt_voice_play(vp_index, false, sync_delay_time, level, ringtone, repeat, callback, 0);
    } else {
        if (role == BT_AWS_MCE_ROLE_AGENT) {
            return app_voice_prompt_voice_play(vp_index, true, sync_delay_time, level, ringtone, repeat, callback, 0);
        } else {
            /* Ignore on partner/client. */
            APPS_LOG_MSGID_I(LOG_TAG"not agent, ignore sync voice", 0);
            return 0;
        }
    }
#else
    return app_voice_prompt_voice_play(vp_index, false, sync_delay_time, level, ringtone, repeat, callback, 0);
#endif
#else
    return 0;
#endif
}

bool apps_config_stop_voice(bool need_sync, uint32_t sync_delay_time,  bool ringtone)
{
#ifdef MTK_PROMPT_SOUND_ENABLE
    app_vp_type cur_type = app_voice_prompt_get_current_type();
    if (cur_type != VP_TYPE_VB && cur_type != VP_TYPE_RT) {
        APPS_LOG_MSGID_I(LOG_TAG"voice not playing, skip stop", 0);
        return 0;
    }
#ifdef MTK_AWS_MCE_ENABLE
    uint32_t role = bt_device_manager_aws_local_info_get_role();
    bt_cm_power_state_t bt_power_state = bt_cm_power_get_state();

    APPS_LOG_MSGID_I(LOG_TAG" stop voice", 0);

    if (role == BT_AWS_MCE_ROLE_NONE || (!need_sync) || (bt_power_state != BT_CM_POWER_STATE_ON)) {
        return app_voice_prompt_voice_stop(false, 0, ringtone);
    } else {
        if (role == BT_AWS_MCE_ROLE_AGENT) {
            return app_voice_prompt_voice_stop(need_sync, sync_delay_time, ringtone);
        } else {
            /* Ignore on partner/client. */
            APPS_LOG_MSGID_I(LOG_TAG"not agent, ignore sync stop voice", 0);
            return 0;
        }
    }
#else
    return app_voice_prompt_voice_stop(false, 0, ringtone);
#endif
#else
    return false;
#endif
}

bool apps_config_set_vp_lang(uint8_t index, bool need_sync)
{
#ifdef MTK_PROMPT_SOUND_ENABLE
    return app_voice_prompt_setLang(index, need_sync);
#else
    return false;
#endif
}

