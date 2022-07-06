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
 * File: app_hfp_activity.c
 *
 * Description:
 * This file is the activity to handle the key action or sink service event when call is active.
 * When the call is active or there is a call coming in, the app_hfp_idle_activity start this
 * activity to handle the call state change events and UI events of HFP.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */


#include "app_hfp_activity.h"
#include "app_hfp_va_activity.h"
#include "apps_events_event_group.h"
#include "apps_config_vp_manager.h"
#include "apps_config_vp_index_list.h"
#include "apps_config_key_remapper.h"
#include "apps_aws_sync_event.h"
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "app_rho_idle_activity.h"
#endif
#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
#include "app_advance_passthrough.h"
#endif
#include "bt_device_manager.h"
#include "bt_aws_mce_srv.h"
#include "bt_device_manager.h"

#define LOG_TAG     "[HFP_APP][VA]"


static bool _proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true */
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(LOG_TAG", create : 0x%x", 1, extra_data);
            self->local_context = extra_data;
            hfp_context_t *hfp_context = (hfp_context_t *)(self->local_context);
            if (hfp_context == NULL) {
                break;
            }
            /* Trigger MMI updating. */
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0, NULL, 0);
#ifdef MTK_AWS_MCE_ENABLE
            if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()) {
                apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                    APPS_EVENTS_INTERACTION_SYNC_VA_STATE,
                                    (void *)&hfp_context->voice_assistant,
                                    sizeof(hfp_context->voice_assistant));
            }
#endif
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(LOG_TAG", destroy", 0);
            break;
        }

        default:
            ret = false;
            break;
    }
    return ret;
}

bool app_hfp_va_activity_proc(
    struct _ui_shell_activity *self,
    uint32_t event_group,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM: {
            /* ui_shell internal events, please refer to doc/Airoha_IoT_SDK_UI_Framework_Developers_Guide.pdf. */
            ret = _proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            hfp_context_t *hfp_context = (hfp_context_t *)(self->local_context);
            if (hfp_context) {
                if (APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE == event_id) {
                    APPS_LOG_MSGID_I(LOG_TAG",APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, voice_assistant: %d", 1, hfp_context->voice_assistant);
                    if (hfp_context->voice_assistant) {
                        apps_config_key_set_mmi_state(APP_STATE_VA);
                        ret = true;
#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
                        app_advance_passthrough_set_va_ongoing(TRUE);
#endif
                    } else {
#ifdef MTK_AWS_MCE_ENABLE
                        if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()) {
                            apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_SYNC_VA_STATE,
                                (void *)&(hfp_context->voice_assistant),
                                sizeof(hfp_context->voice_assistant));
                        }
#endif
#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
                        app_advance_passthrough_set_va_ongoing(FALSE);
#endif
                        ui_shell_finish_activity(self, self);
                        ret = false;
                    }
                }
            }
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_SINK: {
            hfp_context_t *hfp_context = (hfp_context_t *)(self->local_context);
            switch (event_id) {
                 case BT_SINK_SRV_EVENT_STATE_CHANGE: {
                     bt_sink_srv_state_change_t *param = (bt_sink_srv_state_change_t *)extra_data;
                     if (param == NULL) {
                         APPS_LOG_MSGID_I(LOG_TAG", state_change :param is null ", 0);
                         return ret;
                     }
                     APPS_LOG_MSGID_I(LOG_TAG" bt_sink_state_change: param_previous=%x, param_now=%x",
                             2, param->previous, param->current);
                     /* Stop va  when call or music active. */
                     if (param->current >= BT_SINK_SRV_STATE_STREAMING) {
                         APPS_LOG_MSGID_I(LOG_TAG", state_change: finish va:", 0);
                         hfp_context->voice_assistant = false;
                         ui_shell_finish_activity(self, self);
                         ret = false;
                     }
                     break;
                 }
            }
            break;
        }
        default:
            break;
    }
    return ret;
}

