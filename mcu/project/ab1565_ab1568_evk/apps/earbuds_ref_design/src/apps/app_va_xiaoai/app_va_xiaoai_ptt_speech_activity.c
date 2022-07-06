
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
 * File: app_va_xiaoai_ptt_speech_activity.c
 *
 * Description:
 * This file is XiaoAI PTT Speech activity.
 * Start the activity when XiaoAI is recording via PTT.
 *
 */

#if defined(MTK_VA_XIAOAI_ENABLE) && defined(MTK_XIAOAI_PTT_ENABLE)

#include "app_va_xiaoai_ptt_speech_activity.h"
#include "app_va_xiaoai_ble_adv.h"
#include "apps_events_interaction_event.h"
#include "apps_events_event_group.h"
#include "apps_config_state_list.h"
#include "apps_config_key_remapper.h"
#include "apps_debug.h"
#include "ui_shell_manager.h"

#define APP_VA_XIAOAI_PTT_TAG           "[XIAOAI_VA][PTT]"

bool app_va_xiaoai_ptt_sppech_activity_proc_ui_shell_group(ui_shell_activity_t *self,
                                                           uint32_t event_id,
                                                           void *extra_data,
                                                           size_t data_len)
{
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(APP_VA_XIAOAI_PTT_TAG" PTT activity CREATE", 0);
            ui_shell_send_event(FALSE,
                                EVENT_PRIORITY_HIGH,
                                EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE,
                                NULL,
                                0,
                                NULL,
                                0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(APP_VA_XIAOAI_PTT_TAG" PTT activity DESTROY", 0);
            ui_shell_send_event(FALSE,
                                EVENT_PRIORITY_HIGH,
                                EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE,
                                NULL,
                                0,
                                NULL,
                                0);
            break;
        }
        default:
            break;
    }
    return TRUE;
}

bool app_va_xiaoai_ptt_sppech_activity_proc(struct _ui_shell_activity *self,
                                            uint32_t event_group,
                                            uint32_t event_id,
                                            void *extra_data,
                                            size_t data_len)
{
    bool ret = FALSE;
    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM: {
            ret = app_va_xiaoai_ptt_sppech_activity_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            if (event_id == APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE) {
                APPS_LOG_MSGID_I(APP_VA_XIAOAI_PTT_TAG" MMI APP_STATE_VA", 0);
                apps_config_key_set_mmi_state(APP_STATE_VA);
                ret = TRUE;
            }
            break;
        }
        case EVENT_GROUP_UI_SHELL_XIAOAI: {
            if (event_id == XIAOAI_EVENT_FINISH_PTT_ACTIVITY) {
                APPS_LOG_MSGID_I(APP_VA_XIAOAI_PTT_TAG" Finish PTT Speech Activity", 0);
                ui_shell_finish_activity(self, self);
                ret = TRUE;
            }
            break;
        }

    }
    return ret;
}

#endif /* MTK_VA_XIAOAI_ENABLE && MTK_XIAOAI_PTT_ENABLE */
